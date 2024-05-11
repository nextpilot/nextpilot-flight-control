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

static rt_timer_t _timer = NULL;

/**
 * Fetch a never-wrapping absolute time value in microseconds from
 * some arbitrary epoch shortly after system start.
 */
RT_WEAK hrt_abstime hrt_absolute_time(void) {
    uint64_t tick = rt_tick_get();
    return 1000000ULL * tick / RT_TICK_PER_SECOND;
}

extern int hrt_tim_isr(uint32_t status);

static void hrt_tim_timeout(void *param) {
    hrt_tim_isr(1);
}

int hrt_tim_expiry(uint32_t timeout_us) {
    if (!_timer) {
        return -1;
    }

    rt_err_t ret = 0;


    if (_timer->parent.flag & RT_TIMER_FLAG_ACTIVATED) {
        ret += rt_timer_stop(_timer);
    }

    rt_int32_t timeout_tick = rt_tick_from_millisecond(timeout_us / 1000);

    ret += rt_timer_control(_timer, RT_TIMER_CTRL_SET_TIME, &timeout_tick);
    ret += rt_timer_start(_timer);


    return ret;
}

int hrt_tim_init(void) {
    _timer = rt_timer_create("hrtimer", hrt_tim_timeout, NULL, 1, RT_TIMER_FLAG_ONE_SHOT);
    return 0;
}

INIT_SUBSYS_EXPORT(hrt_tim_init);
