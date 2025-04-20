/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <hrtimer.h>

static rt_timer_t _delay_timer  = NULL;
static rt_timer_t _period_timer = NULL;

int hrt_call_isr_after(uint32_t timeout_us) {
    if (!_delay_timer) {
        return -1;
    }

    rt_err_t ret = 0;


    if (_delay_timer->parent.flag & RT_TIMER_FLAG_ACTIVATED) {
        ret += rt_timer_stop(_delay_timer);
    }

    rt_int32_t tick = rt_tick_from_millisecond(timeout_us / 1000);

    if (tick == 0) {
        tick = 1;
    }

    ret += rt_timer_control(_delay_timer, RT_TIMER_CTRL_SET_TIME, &tick);
    ret += rt_timer_start(_delay_timer);


    return ret;
}

int hrt_call_isr_every(uint32_t period_us) {
    if (!_period_timer) {
        return -1;
    }

    if (period_us == 0) {
        return rt_timer_stop(_period_timer);
    }

    rt_err_t ret = 0;

    // if (_period_timer->parent.flag & RT_TIMER_FLAG_ACTIVATED) {
    //     ret += rt_timer_stop(_period_timer);
    // }

    rt_int32_t tick = rt_tick_from_millisecond(period_us / 1000);

    ret += rt_timer_control(_period_timer, RT_TIMER_CTRL_SET_TIME, &tick);
    // ret += rt_timer_start(_period_timer);

    return ret;
}

static void hrt_tim_timeout(void *param) {
    hrt_call_isr(1);
}

int hrt_tim_init(void) {
    // 延时定时器
    _delay_timer = rt_timer_create("hrt_delay", hrt_tim_timeout, NULL, 1, RT_TIMER_FLAG_ONE_SHOT);
    // 100ms周期定时器
    _period_timer = rt_timer_create("hrt_period", hrt_tim_timeout, NULL, 100, RT_TIMER_FLAG_PERIODIC);

    if (_delay_timer && _period_timer) {
        return 0;
    } else {
        return -1;
    }
}

INIT_SUBSYS_EXPORT(hrt_tim_init);
