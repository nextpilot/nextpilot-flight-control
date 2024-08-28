/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "test_hrtimer"
#define LOG_LVL LOG_LVL_INFO

#include "hrtimer.h"
#include "rtdbg.h"


hrt_call hrt_call_every_call{};

static int hrt_call_every_delay  = 0;
static int hrt_call_every_period = 1000;
static int hrt_call_every_count  = 10;

static int test_hrt_call_every_func(void *param) {
    static int      total_count = 0;
    static uint64_t last_time   = 0;
    // 周期统计
    uint64_t now = hrt_absolute_time();
    if (now - last_time > hrt_call_every_period + 100) {
        LOG_E("call period too long");
    }
    last_time = now;
    // 次数统计
    total_count++;
    if (total_count > hrt_call_every_count) {
        total_count                = 0;
        hrt_call_every_call.period = 0;
    }
}

static int test_hrt_call_every_main(int argc, char *argv) {
    if (argc != 4) {
        rt_kprintf("usage: hrt_call_every period_us delay_us count\n");
        return -1;
    }

    hrt_call_every_period = strtol(&argv[1], nullptr, 10);
    hrt_call_every_delay  = strtol(&argv[2], nullptr, 10);
    hrt_call_every_count  = strtol(&argv[3], nullptr, 10);

    rt_kprintf("period=%d,delay=%d,count=%d\n", hrt_call_every_period, hrt_call_every_delay, 10);
    hrt_call_every(&hrt_call_every_call, hrt_call_every_delay, hrt_call_every_period, test_hrt_call_every_func, nullptr);
    return 0;
}

MSH_CMD_EXPORT_ALIAS(test_hrt_call_every_main, hrt_call_every, test hrt_call_every);
