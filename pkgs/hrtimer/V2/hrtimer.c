/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <rtthread.h>
#include <rtdbg.h>
#include "hrtimer.h"

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
static struct sq_queue_s callout_queue;

/* latency baseline (last compare value applied) */
static uint64_t latency_baseline;

/* timer count at interrupt (for latency purposes) */
static uint64_t latency_actual;

/* latency histogram */
const uint16_t latency_bucket_count                  = LATENCY_BUCKET_COUNT;
const uint16_t latency_buckets[LATENCY_BUCKET_COUNT] = {1, 2, 5, 10, 20, 50, 100, 1000};
uint32_t       latency_counters[LATENCY_BUCKET_COUNT + 1];

/* timer-specific functions */
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

/**
 * Initialise the timer we are going to use.
 *
 * We expect that we'll own one of the reduced-function STM32 general
 * timers, and that we can use channel 1 in compare mode.
 */
RT_WEAK int hrt_tim_init(void) {
    return 0;
}

/**
 * Initialise the high-resolution timing module.
 */
static int hrt_init(void) {
    sq_init(&callout_queue);

    hrt_tim_init();

    return 0;
}

INIT_COMPONENT_EXPORT(hrt_init);

/*
 * Get absolute time.
 */
// RTT提供了以下获取高精度时间的函数：
// 1. gettimeofday，需要开启RT_USING_RTC，返回timeval，但是RTT只是实现到sec
// 2. clock_gettime，需要开启式RT_USING_RTC和RT_USING_POSIX_CLOCK，返回timespec，但是RTT只是实现到ms
//
RT_WEAK hrt_abstime hrt_absolute_time(void) {
    uint64_t tick = rt_tick_get();
    return 1000000ULL * tick / RT_TICK_PER_SECOND;
}

/*
 * Convert a timespec to absolute time.
 */
hrt_abstime ts_to_abstime(const struct timespec *ts) {
    hrt_abstime result;
    result  = (hrt_abstime)(ts->tv_sec) * 1000000ULL;
    result += ts->tv_nsec / 1000ULL;
    return result;
}

/*
 * Compute the delta between a timestamp taken in the past
 * and now.
 *
 * This function is safe to use even if the timestamp is updated
 * by an interrupt during execution.
 */
hrt_abstime hrt_elapsed_time_atomic(const volatile hrt_abstime *then) {
    // This is not atomic as the value on the application layer of POSIX is limited.
    rt_enter_critical();
    hrt_abstime delta = hrt_absolute_time() - *then;
    rt_exit_critical();
    return delta;
}

/*
 * Store the absolute time in an interrupt-safe fashion.
 *
 * This function ensures that the timestamp cannot be seen half-written by an interrupt handler.
 */
void hrt_store_absolute_time(volatile hrt_abstime *t) {
    rt_enter_critical();
    *t = hrt_absolute_time();
    rt_exit_critical();
}

void abstime_to_ts(struct timespec *ts, hrt_abstime abstime) {
    ts->tv_sec   = abstime / 1000000ULL;
    abstime     -= ts->tv_sec * 1000000ULL;
    ts->tv_nsec  = abstime * 1000ULL;
}

/**
 * Timer interrupt handler
 *
 * This routine simulates a timer interrupt handler
 */
void hrt_tim_isr(uint32_t status) {
    // rt_enter_critical();

    /* grab the timer for latency tracking purposes */
    latency_actual = hrt_absolute_time();

    if (status == 0) {
        /* do latency calculations */
        hrt_latency_update();

        /* run any callouts that have met their deadline */
        hrt_call_invoke();

        /* and schedule the next interrupt */
        hrt_call_reschedule();
    }
    // rt_exit_critical();
}

/*
 * If this returns true, the entry has been invoked and removed from the callout list,
 * or it has never been entered.
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
    rt_enter_critical();

    sq_rem(&entry->link, &callout_queue);
    entry->deadline = 0;

    /* if this is a periodic call being removed by the callout, prevent it from
     * being re-entered when the callout returns.
     */
    entry->period = 0;

    rt_exit_critical();
}

/*
 * Call callout(arg) after delay has elapsed.
 *
 * If callout is nullptr, this can be used to implement a timeout by testing the call
 * with hrt_called().
 */
void hrt_call_after(struct hrt_call *entry, hrt_abstime delay, hrt_callout callout, void *arg) {
    hrt_call_internal(entry,
                      hrt_absolute_time() + delay,
                      0,
                      callout,
                      arg);
}

/*
 * Call callout(arg) at absolute time calltime.
 */
void hrt_call_at(struct hrt_call *entry, hrt_abstime calltime, hrt_callout callout, void *arg) {
    hrt_call_internal(entry, calltime, 0, callout, arg);
}

/*
 * Call callout(arg) after delay, and then after every interval.
 *
 * Note thet the interval is timed between scheduled, not actual, call times, so the call rate may
 * jitter but should not drift.
 */
void hrt_call_every(struct hrt_call *entry,
                    hrt_abstime      delay,
                    hrt_abstime      interval,
                    hrt_callout      callout,
                    void            *arg) {
    hrt_call_internal(entry,
                      hrt_absolute_time() + delay,
                      interval,
                      callout,
                      arg);
}

static void hrt_call_internal(struct hrt_call *entry,
                              hrt_abstime      deadline,
                              hrt_abstime      interval,
                              hrt_callout      callout,
                              void            *arg) {
    rt_enter_critical();

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

    rt_exit_critical();
}

static void hrt_call_enter(struct hrt_call *entry) {
    struct hrt_call *call, *next;

    call = (struct hrt_call *)sq_peek(&callout_queue);

    if ((call == NULL) || (entry->deadline < call->deadline)) {
        sq_addfirst(&entry->link, &callout_queue);
        // LOG_D("call enter at head, reschedule");
        /* we changed the next deadline, reschedule the timer event */
        hrt_call_reschedule();

    } else {
        do {
            next = (struct hrt_call *)sq_next(&call->link);

            if ((next == NULL) || (entry->deadline < next->deadline)) {
                // LOG_D("call enter after head");
                sq_addafter(&call->link, &entry->link, &callout_queue);
                break;
            }
        } while ((call = next) != NULL);
    }

    // LOG_D("scheduled");
}

static void hrt_call_invoke(void) {
    struct hrt_call *call;
    hrt_abstime      deadline;

    // hrt_lock();

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
        // LOG_D("call pop");

        /* invoke the callout (if there is one) */
        if (call->callout) {
            // Unlock so we don't deadlock in callback
            // hrt_unlock();

            // LOG_D("call %p: %p(%p)", call, call->callout, call->arg);
            call->callout(call->arg);

            // hrt_lock();
        }

        /* save the intended deadline for periodic calls */
        deadline = call->deadline;

        /* zero the deadline, as the call has occurred */
        call->deadline = 0;

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

    // hrt_unlock();
}

/**
 * Reschedule the next timer interrupt.
 *
 * This routine must be called with interrupts disabled.
 */
static void hrt_call_reschedule() {
    hrt_abstime      now      = hrt_absolute_time();
    hrt_abstime      delay    = HRT_INTERVAL_MAX;
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
        // LOG_D("entry in queue");
        if (next->deadline <= (now + HRT_INTERVAL_MIN)) {
            // LOG_D("pre-expired");
            /* set a minimal deadline so that we call ASAP */
            // deadline = now + HRT_INTERVAL_MIN;
            delay = HRT_INTERVAL_MIN;

        } else if (next->deadline < deadline) {
            // LOG_D("due soon");
            // deadline = next->deadline;
            delay = next->deadline - now;
        }
    }

    /* set the new compare value and remember it for latency tracking */
    latency_baseline = now + delay;

    // LOG_D("schedule for %u at %u", (unsigned)(deadline & 0xffffffff), (unsigned)(now & 0xffffffff));

#if defined(HRT_USING_HARD_TIMER)
    /* set the new compare value and remember it for latency tracking */
    // rCCR_HRT = latency_baseline = deadline & 0xffff;
#else
    // do nothing
#endif // HRT_USING_HARD_TIMER
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
    rt_memset(entry, 0, sizeof(*entry));
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
