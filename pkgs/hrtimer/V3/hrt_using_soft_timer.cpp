/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include <rtthread.h>
#include <hrtimer.h>

static rt_timer_t _timer = nullptr;

/**
 * Fetch a never-wrapping absolute time value in microseconds from
 * some arbitrary epoch shortly after system start.
 */
hrt_abstime hrt_absolute_time(void) {
    uint64_t tick = rt_tick_get();
    return 1000000ULL * tick / RT_TICK_PER_SECOND;
}

static void hrt_tim_timeout(void *param) {
    hrt_tim_isr(1);
}

int hrt_tim_control(int cmd, void *arg) {
    if (_timer) {
        return rt_timer_control(_timer, cmd, arg);
    }
    return -1;
}

int hrt_tim_init(void) {
    _timer = rt_timer_create("hrtimer", hrt_tim_timeout, nullptr, 5, RT_TIMER_FLAG_PERIODIC);
    return 0;
}
