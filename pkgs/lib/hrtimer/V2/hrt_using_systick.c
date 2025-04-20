
#include "hrtimer.h"
#include "board.h"

static uint64_t _timebase_us = 0;

hrt_abstime hrt_absolute_time(void) {
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
}

int hrt_tim_init(void) {
    static struct rt_timer hrt_call_timer;
    //rt_timer_init(&hrt_call_timer, "hrt_call_timer", hrt_tim_isr, NULL, 1, RT_TIMER_FLAG_PERIODIC);
    //rt_timer_start(&hrt_call_timer);
    return 0;
}