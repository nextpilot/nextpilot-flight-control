/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "sim_rate"
#define LOG_LVL LOG_LVL_INFO

#include <nextpilot.h>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/vehicle_angular_velocity.h>

using namespace time_literals;
using namespace nextpilot;

/**
 * @brief 模拟角速率控制，基于vehicle_angular_velocity主题的callback驱动run
 *
 */

class SimRateControl : public ModuleCommand<SimRateControl>, public WorkItem {
public:
    SimRateControl(int instance) :
        WorkItem(LOG_TAG, wq_configurations::rate_ctrl) {
        _instance = instance;
    }

    ~SimRateControl() {
    }

    /** @see ModuleCommand */
    static SimRateControl *instantiate(int argc, char *argv[]) {
        int             inst     = atoi(argv[1]);
        SimRateControl *instance = new SimRateControl(inst);
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
        if (!_vehicle_angular_velocity_sub.register_callback()) {
            LOG_E("callback registration failed");
            return -1;
        }
        return 0;
    }

    /* */
    void Run() override {
        static uint32_t cnt   = 0;
        static double   total = 0.0f;
        if (should_exit()) {
        }
        // rt_thread_mdelay(10); workqueue中不能有delay

        vehicle_angular_velocity_s angular_velocity;
        if (_vehicle_angular_velocity_sub.update(&angular_velocity)) {
            if ((_instance == 0) && (cnt % 20 == 0)) {
                rt_tick_t current_tick = rt_tick_get();
                LOG_I("[SimRateControl]>>>instance=%d, t=%ld, dt=%ld, cnt=%d\n", _instance, current_tick, current_tick - _last_tick, cnt);
                LOG_I("[SimRateControl]>>>msg pub time=%llu\n", angular_velocity.timestamp);
                _last_tick = current_tick;
            }
            cnt++;
        }
    }

private:
    int _instance;

    rt_tick_t _last_tick;
    //
    uORB::SubscriptionCallbackWorkItem _vehicle_angular_velocity_sub{this, ORB_ID(vehicle_angular_velocity)};
};

int sim_rate_ctrl_start() {
    int         ret;
    const char *argv[] = {"sim_rate_control", "start", "0"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    ret                = SimRateControl::main(argc, (char **)argv);

    return ret;
}

INIT_APP_EXPORT(sim_rate_ctrl_start);
