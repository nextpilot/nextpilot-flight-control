#include <nextpilot.h>
#include <workq/WorkItemScheduled.hpp>

using namespace time_literals;
using namespace nextpilot;

static wq_config_t sim_rate_ctrl{"wq:sim_rate_ctrl", 4096 * 10, 0};

class SimRateControl : public ModuleCommand<SimRateControl>, public nextpilot::WorkItemScheduled {
public:
    SimRateControl() :
        WorkItemScheduled(MODULE_NAME, sim_rate_ctrl) {
    }

    ~SimRateControl() {
    }

    /** @see ModuleCommand */
    static SimRateControl *instantiate(int argc, char *argv[]) {
        SimRateControl *instance = new SimRateControl();
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
        rt_thread_mdelay(1000);
        rt_kprintf("======%ld, %d\n", rt_tick_get(), cnt++);
    }
};

int sim_rate_ctrl_start() {
    const char *argv[] = {"sim_rate_control", "start"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    return SimRateControl::main(argc, (char **)argv);
}

INIT_APP_EXPORT(sim_rate_ctrl_start);
