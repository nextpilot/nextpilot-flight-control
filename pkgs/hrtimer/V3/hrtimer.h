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
 * @file drv_hrt.h
 *
 * High-resolution timer with callouts and timekeeping.
 */

#pragma once

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdbool.h>
#include <inttypes.h>
#include <queue.h>
#include <rtthread.h>
#include <defines.h>


__BEGIN_DECLS

/**
 * Absolute time, in microsecond units.
 *
 * Absolute time is measured from some arbitrary epoch shortly after
 * system startup.  It should never wrap or go backwards.
 */
typedef uint64_t hrt_abstime;

/**
 * Callout function type.
 *
 * Note that callouts run in the timer interrupt context, so
 * they are serialised with respect to each other, and must not
 * block.
 */
typedef void (*hrt_callout)(void *arg);

/**
 * Callout record.
 */
typedef struct hrt_call {
    struct sq_entry_s link;

    hrt_abstime deadline;
    hrt_abstime period;
    hrt_callout callout;
    void       *arg;
#if defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
    hrt_callout usr_callout;
    void       *usr_arg;
#endif
} *hrt_call_t;

#define LATENCY_BUCKET_COUNT 8
extern const uint16_t latency_bucket_count;
extern const uint16_t latency_buckets[LATENCY_BUCKET_COUNT];
extern uint32_t       latency_counters[LATENCY_BUCKET_COUNT + 1];

typedef struct latency_info {
    uint16_t bucket;
    uint32_t counter;
} latency_info_t;

#if defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)

typedef struct hrt_boardctl {
    hrt_call_t  entry;
    hrt_abstime time; /* delay or calltime */
    hrt_abstime interval;
    hrt_callout callout;
    void       *arg;
} hrt_boardctl_t;

typedef struct latency_boardctl {
    uint16_t       bucket_idx;
    uint16_t       counter_idx;
    latency_info_t latency;
} latency_boardctl_t;

#   define _HRTIOC(_n)       (_PX4_IOC(_HRTIOCBASE, _n))

#   define HRT_WAITEVENT     _HRTIOC(1)
#   define HRT_ABSOLUTE_TIME _HRTIOC(2)
#   define HRT_CALL_AFTER    _HRTIOC(3)
#   define HRT_CALL_AT       _HRTIOC(4)
#   define HRT_CALL_EVERY    _HRTIOC(5)
#   define HRT_CANCEL        _HRTIOC(6)
#   define HRT_GET_LATENCY   _HRTIOC(7)
#   define HRT_RESET_LATENCY _HRTIOC(8)

#endif

/**
 * Get absolute time in [us] (does not wrap).
 */
__EXPORT extern hrt_abstime hrt_absolute_time(void);

/**
 * Convert a timespec to absolute time.
 */
static inline hrt_abstime ts_to_abstime(const struct timespec *ts) {
    hrt_abstime result;

    result  = (hrt_abstime)(ts->tv_sec) * 1000000;
    result += (hrt_abstime)(ts->tv_nsec / 1000);

    return result;
}

/**
 * Convert absolute time to a timespec.
 */
static inline void abstime_to_ts(struct timespec *ts, hrt_abstime abstime) {
    ts->tv_sec   = /*(typeof(ts->tv_sec))*/ (abstime / 1000000);
    abstime     -= (hrt_abstime)(ts->tv_sec) * 1000000;
    ts->tv_nsec  = /*(typeof(ts->tv_nsec))*/ (abstime * 1000);
}

/**
 * Compute the delta between a timestamp taken in the past
 * and now.
 *
 * This function is not interrupt save.
 */
static inline hrt_abstime hrt_elapsed_time(const hrt_abstime *then) {
    return hrt_absolute_time() - *then;
}

/**
 * Store the absolute time in an interrupt-safe fashion.
 *
 * This function ensures that the timestamp cannot be seen half-written by an interrupt handler.
 */
static inline void hrt_store_absolute_time(volatile hrt_abstime *t) {
    rt_base_t flags = rt_hw_interrupt_disable();
    *t              = hrt_absolute_time();
    rt_hw_interrupt_enable(flags);
}

#ifdef __PX4_QURT
/**
 * Set a time offset to hrt_absolute_time on the DSP.
 * @param time_diff_us: time difference of the DSP clock to Linux clock.
 *   This param is positive if the Linux clock is ahead of the DSP one.
 */
__EXPORT extern int hrt_set_absolute_time_offset(int32_t time_diff_us);
#endif

/**
 * Call callout(arg) after delay has elapsed.
 *
 * If callout is NULL, this can be used to implement a timeout by testing the call
 * with hrt_called().
 */
__EXPORT extern void hrt_call_after(struct hrt_call *entry, hrt_abstime delay, hrt_callout callout, void *arg);

/**
 * Call callout(arg) at absolute time calltime.
 */
__EXPORT extern void hrt_call_at(struct hrt_call *entry, hrt_abstime calltime, hrt_callout callout, void *arg);

/**
 * Call callout(arg) after delay, and then after every interval.
 *
 * Note thet the interval is timed between scheduled, not actual, call times, so the call rate may
 * jitter but should not drift.
 */
__EXPORT extern void hrt_call_every(struct hrt_call *entry, hrt_abstime delay, hrt_abstime interval,
                                    hrt_callout callout, void *arg);

/**
 * If this returns true, the entry has been invoked and removed from the callout list,
 * or it has never been entered.
 *
 * Always returns false for repeating callouts.
 */
__EXPORT extern bool hrt_called(struct hrt_call *entry);

/**
 * Remove the entry from the callout list.
 */
__EXPORT extern void hrt_cancel(struct hrt_call *entry);

/**
 * Initialise a hrt_call structure
 */
__EXPORT extern void hrt_call_init(struct hrt_call *entry);

/*
 * delay a hrt_call_every() periodic call by the given number of
 * microseconds. This should be called from within the callout to
 * cause the callout to be re-scheduled for a later time. The periodic
 * callouts will then continue from that new base time at the
 * previously specified period.
 */
__EXPORT extern void hrt_call_delay(struct hrt_call *entry, hrt_abstime delay);

/*
 * update with the new expiry
 */
__EXPORT extern int hrt_call_isr_after(uint32_t timeout_us);
__EXPORT extern int hrt_call_isr_every(uint32_t period_us);

__EXPORT extern int hrt_call_isr(int status);

#if defined(ENABLE_LOCKSTEP_SCHEDULER)

__EXPORT extern int
                     lockstep_register_component(void);
__EXPORT extern void lockstep_unregister_component(int component);
__EXPORT extern void lockstep_progress(int component);
__EXPORT extern void lockstep_wait_for_components(void);

#else
static inline int lockstep_register_component(void) {
    return 0;
}

static inline void lockstep_unregister_component(int component) {
    (void)component;
}

static inline void lockstep_progress(int component) {
    (void)component;
}

static inline void lockstep_wait_for_components(void) {
}
#endif /* defined(ENABLE_LOCKSTEP_SCHEDULER) */


/* Latency counter functions */

static inline uint16_t get_latency_bucket_count(void) {
    return LATENCY_BUCKET_COUNT;
}

#if defined(CONFIG_BUILD_FLAT) || !defined(__PX4_NUTTX)

static inline latency_info_t get_latency(uint16_t bucket_idx, uint16_t counter_idx) {
    latency_info_t ret = {latency_buckets[bucket_idx], latency_counters[counter_idx]};
    return ret;
}

static inline void reset_latency_counters(void) {
    for (int i = 0; i <= get_latency_bucket_count(); i++) {
        latency_counters[i] = 0;
    }
}

#else

/* NuttX protected/kernel build interface functions */

latency_info_t get_latency(uint16_t bucket_idx, uint16_t counter_idx);
void           reset_latency_counters(void);

#endif

__END_DECLS


#ifdef __cplusplus

namespace time_literals {

// User-defined integer literals for different time units.
// The base unit is hrt_abstime in microseconds

constexpr hrt_abstime operator"" _s(unsigned long long seconds) {
    return hrt_abstime(seconds * 1000000ULL);
}

constexpr hrt_abstime operator"" _ms(unsigned long long milliseconds) {
    return hrt_abstime(milliseconds * 1000ULL);
}

constexpr hrt_abstime operator"" _us(unsigned long long microseconds) {
    return hrt_abstime(microseconds);
}

} /* namespace time_literals */


#endif /* __cplusplus */
