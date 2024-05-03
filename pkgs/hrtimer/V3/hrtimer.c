/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file drv_hrt.c
 *
 * High-resolution timer callouts and timekeeping.
 *
 * This can use any general or advanced STM32 timer.
 *
 */

#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <queue.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <hrtimer.h>


#ifdef CONFIG_DEBUG_HRT
#   define hrtinfo _info
#else
#   define hrtinfo(x...)
#endif


/**
 * Minimum/maximum deadlines.
 *
 * These are suitable for use with a 16-bit timer/counter clocked
 * at 1MHz.  The high-resolution timer need only guarantee that it
 * not wrap more than once in the 50ms period for absolute time to
 * be consistently maintained.
 *
 * The minimum deadline must be such that the time taken between
 * reading a time and writing a deadline to the timer cannot
 * result in missing the deadline.
 */
#define HRT_INTERVAL_MIN 50
#define HRT_INTERVAL_MAX 50000


/*
 * Queue of callout entries.
 */
static struct sq_queue_s callout_queue = {NULL, NULL};

/* latency baseline (last compare value applied) */
static uint16_t latency_baseline;

/* timer count at interrupt (for latency purposes) */
static uint16_t latency_actual;

/* latency histogram */
const uint16_t    latency_bucket_count                  = LATENCY_BUCKET_COUNT;
const uint16_t    latency_buckets[LATENCY_BUCKET_COUNT] = {1, 2, 5, 10, 20, 50, 100, 1000};
__EXPORT uint32_t latency_counters[LATENCY_BUCKET_COUNT + 1];

/* timer-specific functions */
// static void hrt_tim_init(void);
// static int  hrt_tim_isr(int irq, void *context, void *arg);
static void hrt_latency_update(void);

/* callout list manipulation */
static void hrt_call_internal(struct hrt_call *entry,
                              hrt_abstime      deadline,
                              hrt_abstime      interval,
                              hrt_callout      callout,
                              void            *arg);
static void hrt_call_enter(struct hrt_call *entry);
static void hrt_call_reschedule(void);
static void hrt_call_invoke(void);

// int hrt_ioctl(unsigned int cmd, unsigned long arg);


/**
 * Handle the compare interrupt by calling the callout dispatcher
 * and then re-scheduling the next deadline.
 */
int hrt_tim_isr(uint32_t status) {
    /* grab the timer for latency tracking purposes */
    latency_actual = hrt_absolute_time();


#ifdef HRT_PPM_CHANNEL

    /* was this a PPM edge? */
    if (status & (SR_INT_PPM | SR_OVF_PPM)) {
        /* if required, flip edge sensitivity */
#   ifdef PPM_EDGE_FLIP
        rCCER ^= CCER_PPM_FLIP;
#   endif

        hrt_ppm_decode(status);
    }

#endif

    /* was this a timer tick? */
    if (status == 1) {
        /* do latency calculations */
        hrt_latency_update();

        /* run any callouts that have met their deadline */
        hrt_call_invoke();

        /* and schedule the next interrupt */
        hrt_call_reschedule();
    }

    return 0;
}

/**
 * Call callout(arg) after interval has elapsed.
 */
void hrt_call_after(struct hrt_call *entry, hrt_abstime delay, hrt_callout callout, void *arg) {
    hrt_call_internal(entry,
                      hrt_absolute_time() + delay,
                      0,
                      callout,
                      arg);
}

/**
 * Call callout(arg) at calltime.
 */
void hrt_call_at(struct hrt_call *entry, hrt_abstime calltime, hrt_callout callout, void *arg) {
    hrt_call_internal(entry, calltime, 0, callout, arg);
}

/**
 * Call callout(arg) every period.
 */
void hrt_call_every(struct hrt_call *entry, hrt_abstime delay, hrt_abstime interval, hrt_callout callout, void *arg) {
    hrt_call_internal(entry,
                      hrt_absolute_time() + delay,
                      interval,
                      callout,
                      arg);
}

static void hrt_call_internal(struct hrt_call *entry, hrt_abstime deadline, hrt_abstime interval, hrt_callout callout, void *arg) {
    rt_base_t flags = rt_hw_interrupt_disable();

    /* if the entry is currently queued, remove it */
    /* note that we are using a potentially uninitialised
	   entry->link here, but it is safe as sq_rem() doesn't
	   dereference the passed node unless it is found in the
	   list. So we potentially waste a bit of time searching the
	   queue for the uninitialised entry->link but we don't do
	   anything actually unsafe.
	*/
    if (entry->deadline != 0) {
        sq_rem(&entry->link, &callout_queue);
    }

    entry->deadline = deadline;
    entry->period   = interval;
    entry->callout  = callout;
    entry->arg      = arg;

    hrt_call_enter(entry);

    rt_hw_interrupt_enable(flags);
}

/**
 * If this returns true, the call has been invoked and removed from the callout list.
 *
 * Always returns false for repeating callouts.
 */
bool hrt_called(struct hrt_call *entry) {
    return (entry->deadline == 0);
}

/**
 * Remove the entry from the callout list.
 */
void hrt_cancel(struct hrt_call *entry) {
    rt_base_t flags = rt_hw_interrupt_disable();

    sq_rem(&entry->link, &callout_queue);
    entry->deadline = 0;

    /* if this is a periodic call being removed by the callout, prevent it from
	 * being re-entered when the callout returns.
	 */
    entry->period = 0;

    rt_hw_interrupt_enable(flags);
}

static void hrt_call_enter(struct hrt_call *entry) {
    struct hrt_call *call, *next;

    call = (struct hrt_call *)sq_peek(&callout_queue);

    if ((call == NULL) || (entry->deadline < call->deadline)) {
        sq_addfirst(&entry->link, &callout_queue);
        hrtinfo("call enter at head, reschedule\n");
        /* we changed the next deadline, reschedule the timer event */
        hrt_call_reschedule();

    } else {
        do {
            next = (struct hrt_call *)sq_next(&call->link);

            if ((next == NULL) || (entry->deadline < next->deadline)) {
                hrtinfo("call enter after head\n");
                sq_addafter(&call->link, &entry->link, &callout_queue);
                break;
            }
        } while ((call = next) != NULL);
    }

    hrtinfo("scheduled\n");
}

static void hrt_call_invoke(void) {
    struct hrt_call *call;
    hrt_abstime      deadline;

    while (true) {
        /* get the current time */
        hrt_abstime now = hrt_absolute_time();

        call = (struct hrt_call *)sq_peek(&callout_queue);

        if (call == NULL) {
            break;
        }

        if (call->deadline > now) {
            break;
        }

        sq_rem(&call->link, &callout_queue);
        hrtinfo("call pop\n");

        /* save the intended deadline for periodic calls */
        deadline = call->deadline;

        /* zero the deadline, as the call has occurred */
        call->deadline = 0;

        /* invoke the callout (if there is one) */
        if (call->callout) {
            hrtinfo("call %p: %p(%p)\n", call, call->callout, call->arg);
            call->callout(call->arg);
        }

        /* if the callout has a non-zero period, it has to be re-entered */
        if (call->period != 0) {
            // re-check call->deadline to allow for
            // callouts to re-schedule themselves
            // using hrt_call_delay()
            if (call->deadline <= now) {
                call->deadline = deadline + call->period;
            }

            hrt_call_enter(call);
        }
    }
}

/**
 * Reschedule the next timer interrupt.
 *
 * This routine must be called with interrupts disabled.
 */
static void hrt_call_reschedule() {
    hrt_abstime      now      = hrt_absolute_time();
    struct hrt_call *next     = (struct hrt_call *)sq_peek(&callout_queue);
    hrt_abstime      deadline = now + HRT_INTERVAL_MAX;

    /*
	 * Determine what the next deadline will be.
	 *
	 * Note that we ensure that this will be within the counter
	 * period, so that when we truncate all but the low 16 bits
	 * the next time the compare matches it will be the deadline
	 * we want.
	 *
	 * It is important for accurate timekeeping that the compare
	 * interrupt fires sufficiently often that the base_time update in
	 * hrt_absolute_time runs at least once per timer period.
	 */
    if (next != NULL) {
        hrtinfo("entry in queue\n");

        if (next->deadline <= (now + HRT_INTERVAL_MIN)) {
            hrtinfo("pre-expired\n");
            /* set a minimal deadline so that we call ASAP */
            deadline = now + HRT_INTERVAL_MIN;

        } else if (next->deadline < deadline) {
            hrtinfo("due soon\n");
            deadline = next->deadline;
        }
    }

    hrtinfo("schedule for %u at %u\n", (unsigned)(deadline & 0xffffffff), (unsigned)(now & 0xffffffff));

    /* set the new compare value and remember it for latency tracking */
    latency_baseline = deadline;

    // Remove the existing expiry and update with the new expiry
    hrt_tim_control(RT_TIMER_CTRL_SET_TIME, deadline - now);
}

static void hrt_latency_update(void) {
    uint16_t latency = latency_actual - latency_baseline;
    unsigned index;

    /* bounded buckets */
    for (index = 0; index < LATENCY_BUCKET_COUNT; index++) {
        if (latency <= latency_buckets[index]) {
            latency_counters[index]++;
            return;
        }
    }

    /* catch-all at the end */
    latency_counters[index]++;
}

/*
 * initialise a hrt_call structure
 */
void hrt_call_init(struct hrt_call *entry) {
    memset(entry, 0, sizeof(*entry));
}

/*
 * delay a hrt_call_every() periodic call by the given number of
 * microseconds. This should be called from within the callout to
 * cause the callout to be re-scheduled for a later time. The periodic
 * callouts will then continue from that new base time at the
 * previously specified period.
 */
void hrt_call_delay(struct hrt_call *entry, hrt_abstime delay) {
    entry->deadline = hrt_absolute_time() + delay;
}
