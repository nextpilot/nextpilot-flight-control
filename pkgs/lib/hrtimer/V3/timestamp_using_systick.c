/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

// 在Cortex-M里面DWT(Data Watchpoint and Trace)，是用于系统调试及跟踪，它有一个32位的寄存器叫CYCCNT，
// 它是一个向上的计数器，内核时钟跳动一次，该计数器就加1，当CYCCNT溢出之后， 会清0重新开始向上计数。
//
// 例如STM32F103系列，内核时钟是72M，那计时精度就是1/72M=14ns，最长能记录的时间为60s=2^32/72M，
// 而STM32H7主频一般为400M，则计时精度高达1/400M=2.5ns，最长记录时间为10.74s=2^32/400M
// 而i.MX RT1052主频高达528M，内核跳一次的时间大概为1/528M=1.9ns，最长能记录的时间8.13s=2^32/528M 。


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
