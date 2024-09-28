/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "test_interval"
#define LOG_LVL LOG_LVL_INFO

#include <nextpilot.h>
#include <workq/WorkItemScheduled.hpp>

using namespace time_literals;
using namespace nextpilot::workq;

/**
 * @brief
 *
 * ModuleCommand<UTestWorkqScheduled, 6>: 最多创建6个实例
 */
class UTestWorkqScheduled : public ModuleCommand<UTestWorkqScheduled, 6>, public WorkItemScheduled {
public:
    UTestWorkqScheduled(int idx) :
        WorkItemScheduled(MODULE_NAME, wq_configurations::rate_ctrl) {
        instance = idx;
    }

    ~UTestWorkqScheduled() {
    }

    /** @see ModuleCommand */
    static UTestWorkqScheduled *instantiate(int argc, char *argv[]) {
        int                  idx      = atoi(argv[1]);
        UTestWorkqScheduled *instance = new UTestWorkqScheduled(idx);
        return instance;
    }

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]) {
        return 0;
    }

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr) {
        return 0;
    }

    int init() override {
        ScheduleOnInterval(100_ms); // 8 Hz
        return 0;
    }

    rt_tick_t _last_tick;

private:
    void Run() override {
        static uint32_t cnt   = 0;
        static double   total = 0.0f;
        if (should_exit()) {
        }
        // rt_thread_mdelay(1000);

        /* 1. 计时等待 */
        rt_tick_t start = rt_tick_get();
        // while (rt_tick_get() - start <= 5) {
        //     total += 1.0;
        // }

        /* 2. 执行一些很耗时的操作*/
        // for (uint32_t sleep = 0; sleep < 500000; ++sleep) {
        //     total += 1.0 / sleep;
        // }

        if (cnt % 10 == 0) {
            rt_tick_t current_tick = rt_tick_get();
            rt_kprintf("[wq_scheduled]>>>instance=%d, t=%ld, dt=%ld, cnt=%d\n", instance, current_tick, current_tick - _last_tick, cnt);
            _last_tick = current_tick;
        }
        cnt++;
    }

    int instance;
};

int workq_scheduled_start() {
    int         ret;
    const char *argv[] = {"utest_workq_scheduled", "start", "0"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    ret                = UTestWorkqScheduled::main(argc, (char **)argv);

    // 目前还无法实现两个实例一起运行
    const char *argv1[] = {"utest_workq_scheduled1", "start", "1"};
    int         argc1   = sizeof(argv1) / sizeof(argv1[0]);
    ret                 = UTestWorkqScheduled::main(argc1, (char **)argv1);

    return ret;
}

// INIT_APP_EXPORT(workq_scheduled_start);
