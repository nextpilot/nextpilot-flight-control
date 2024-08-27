#define LOG_TAG "utest_wq"
#define LOG_LVL LOG_LVL_INFO

#include <nextpilot.h>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/vehicle_angular_velocity.h>

using namespace time_literals;
using namespace nextpilot;

static wq_config_t sim_imu{"wq:sim_imu", 4096 * 10, 0};
static wq_config_t sim_rate_ctrl{"wq:sim_rate_ctrl", 4096 * 10, 0};

/**
 * @brief 模拟IMU，用于发布角速率数据
 *
 */
class SimIMU : public ModuleCommand<SimIMU>, public nextpilot::WorkItemScheduled {
public:
    SimIMU(int instance) :
        WorkItemScheduled("hhhhh", sim_rate_ctrl),
        _instance(instance) {
    }

    ~SimIMU() {
    }

    /** @see ModuleCommand */
    static SimIMU *instantiate(int argc, char *argv[]) {
        int     inst     = atoi(argv[1]);
        SimIMU *instance = new SimIMU(inst);
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
        ScheduleOnInterval(5_ms); // 200 Hz
        return 0;
    }

    rt_tick_t tick_buff[50];

    void Run() override {
        hrt_abstime now_us        = hrt_absolute_time();
        _vangvel.timestamp        = now_us;
        _vangvel.timestamp_sample = now_us;
        for (int axis = 0; axis < 3; axis++) {
            _vangvel.xyz[axis]            = 1.0f;
            _vangvel.xyz_derivative[axis] = 0.0f;
        }
        _vangvel_pub.publish(_vangvel);


        static int kk      = 0;
        tick_buff[kk % 50] = rt_tick_get();
        kk++;
        //
        static int cnt = 0;
        if ((_instance == 0) && (cnt % 20 == 0)) {
            rt_tick_t current_tick = rt_tick_get();
            uint64_t  _start       = hrt_absolute_time();
            LOG_I("[SimIMU]>>>instance=%d, t=%ld, dt=%ld, cnt=%d\n", _instance, current_tick, current_tick - _last_tick, cnt);

            uint64_t _delta = hrt_absolute_time() - _start;
            _last_tick      = current_tick;
        }
        cnt++;
    }

private:
    int       _instance{};
    rt_tick_t _last_tick{};

    //
    struct vehicle_angular_velocity_s _vangvel {};

    uORB::Publication<vehicle_angular_velocity_s> _vangvel_pub{ORB_ID(vehicle_angular_velocity)};
};

/**
 * @brief 模拟角速率控制，订阅角速率
 *
 */
class SimRateControl : public ModuleCommand<SimRateControl>, public nextpilot::WorkItem {
public:
    SimRateControl(int instance) :
        WorkItem(MODULE_NAME, sim_rate_ctrl) {
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
            PX4_ERR("callback registration failed");
            return -1;
        }
        return 0;
    }

    rt_tick_t _last_tick;

private:
    int _instance;
    //
    uORB::SubscriptionCallbackWorkItem _vehicle_angular_velocity_sub{this, ORB_ID(vehicle_angular_velocity)};

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
};

// 启动
int sim_imu_start() {
    int         ret;
    const char *argv[] = {"sim_imu", "start", "0"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    ret                = SimIMU::main(argc, (char **)argv);
    return ret;
}

INIT_APP_EXPORT(sim_imu_start);

int sim_rate_ctrl_start() {
    int         ret;
    const char *argv[] = {"sim_rate_control", "start", "0"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    ret                = SimRateControl::main(argc, (char **)argv);

    return ret;
}

INIT_APP_EXPORT(sim_rate_ctrl_start);
