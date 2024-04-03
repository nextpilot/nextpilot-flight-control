/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "hrtimer.h"
#include "board.h"

#ifdef PKG_USING_PERF_COUNTER
#include <perf_counter.h>
#endif // PKG_USING_PERF_COUNTER

#ifdef SOC_FAMILY_STM32
// static uint32_t _sys_clock_freq = 0;
static uint64_t _timebase_us = 0;

int hrt_abstime_init() {
    // SystemCoreClock，好像也可以使用这个变量
    // _sys_clock_freq = HAL_RCC_GetSysClockFreq();
#ifdef RT_USING_RTC_READ_TIME_ERROR
    rt_base_t   level;
    time_t      time = 0;
    rt_tick_t   tick;
    rt_device_t device = rt_device_find("rtc");
    if (device) {
        /* get realtime seconds */
        if (rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time) == RT_EOK) {
            level        = rt_hw_interrupt_disable();
            tick         = rt_tick_get(); /* get tick */
            _timebase_us = 1000000 * time - 1000000 * tick / RT_TICK_PER_SECOND;
            rt_hw_interrupt_enable(level);
            return 0;
        }
    }
    return -1;
#else
    return 0;
#endif
}
INIT_COMPONENT_EXPORT(hrt_abstime_init);
#endif

/*
 * Get absolute time.
 */
// RTT提供了以下获取高精度时间的函数：
// 1. gettimeofday，需要开启RT_USING_RTC，返回timeval，但是RTT只是实现到sec
// 2. clock_gettime，需要开启式RT_USING_RTC和RT_USING_POSIX_CLOCK，返回timespec，但是RTT只是实现到ms
//
hrt_abstime hrt_absolute_time(void) {
#if defined(RT_USING_RTC__HAVE_BUG)
#if defined(RT_USING_POSIX_CLOCK__HAVE_BUG)
    // CLOCK_REALTIME采用了rt_tick，因此其精度也是ms级别
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts_to_abstime(&ts);
#else
    // gettimeofday只是实现了sec，没有实现usec
    struct timeval  now;
    struct timespec ts;
    if (gettimeofday(&now, NULL) != RT_EOK) {
        return 0;
    }
    ts.tv_sec  = now.tv_sec;
    ts.tv_nsec = now.tv_usec * 1000;
    return ts_to_abstime(&ts);
#endif
#elif defined(RT_USING_CPUTIME__HAVE_BUG)
    // 注意clock_cpu_gettime内部是一个uint32的变量，因此精度会溢出
    struct timespec ts;
    float           unit     = clock_cpu_getres();
    long long       cpu_tick = clock_cpu_gettime();
    ts->tv_sec               = ((long long)(cpu_tick * unit)) / NANOSECOND_PER_SECOND;
    ts->tv_nsec              = ((long long)(cpu_tick * unit)) % NANOSECOND_PER_SECOND;
    return ts_to_abstime(&ts);
// #elif defined(PKG_USING_PERF_COUNTER)
//     return get_system_us();
#elif defined(SOC_FAMILY_STM32)
    uint64_t us   = 0;
    uint64_t tick = rt_tick_get();

    uint64_t t1 = (SysTick->LOAD - SysTick->VAL);
    uint64_t t3 = (SysTick->LOAD - SysTick->VAL);

    if (t3 < t1) {
        tick++;
        us = 1000000ULL * t3 / SystemCoreClock;
    } else {
        us = 1000000ULL * t1 / SystemCoreClock;
    }

    return us +                                     // 当前us
           1000000ULL * tick / RT_TICK_PER_SECOND + // rt_tick转us
           _timebase_us;                            // 初始时间戳us
#else
    uint64_t tick = rt_tick_get();
    return 1000000ULL * tick / RT_TICK_PER_SECOND;
#endif
}

/*
 * Convert a timespec to absolute time.
 */
hrt_abstime ts_to_abstime(const struct timespec *ts) {
    hrt_abstime result;
    result = (hrt_abstime)(ts->tv_sec) * 1000000;
    result += ts->tv_nsec / 1000;
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
    ts->tv_sec = abstime / 1000000ULL;
    abstime -= ts->tv_sec * 1000000ULL;
    ts->tv_nsec = abstime * 1000ULL;
}
