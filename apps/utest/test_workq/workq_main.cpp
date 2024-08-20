#include <nextpilot.h>
#include <workq/WorkItemScheduled.hpp>

using namespace time_literals;
using namespace nextpilot;

static wq_config_t sim_rate_ctrl{"wq:sim_rate_ctrl", 4096 * 10, 0};

/**
 * @brief
 *
 * ModuleCommand<SimRateControl, 6>: 最多创建6个实例
 */
class SimRateControl : public ModuleCommand<SimRateControl, 6>, public nextpilot::WorkItemScheduled {
public:
    SimRateControl(int idx) :
        WorkItemScheduled(MODULE_NAME, sim_rate_ctrl) {
        _idx = idx;
    }

    ~SimRateControl() {
    }

    /** @see ModuleCommand */
    static SimRateControl *instantiate(int argc, char *argv[]) {
        int             idx      = atoi(argv[1]);
        SimRateControl *instance = new SimRateControl(idx);
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

private:
    void Run() override {
        static uint32_t cnt = 0;
        if (should_exit()) {
        }
        // rt_thread_mdelay(1000);
        rt_tick_t start = rt_tick_get();
        while (rt_tick_get() - start <= 2) {
        }
        // static double total = 0.0f;
        // // 执行一些很耗时的操作
        // for (uint32_t sleep = 0; sleep < 500000; ++sleep) {
        //     total += 1.0 / sleep;
        // }
        // if (_idx == 0) {
        //     rt_kprintf("======idx=%d, %ld, %d\n", _idx, rt_tick_get(), cnt++);
        // }
    }

    int _idx;
};

int sim_rate_ctrl_start() {
    int         ret;
    const char *argv[] = {"sim_rate_control", "start", "0"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    ret                = SimRateControl::main(argc, (char **)argv);


    // const char *argv1[] = {"sim_rate_control1", "start", "1"};
    // int         argc1   = sizeof(argv1) / sizeof(argv1[0]);
    // ret                 = SimRateControl::main(argc1, (char **)argv1);

    return ret;
}

INIT_APP_EXPORT(sim_rate_ctrl_start);
