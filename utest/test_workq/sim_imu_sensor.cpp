/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#define LOG_TAG "sim_imu"
#define LOG_LVL LOG_LVL_INFO

#include <nextpilot.h>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/vehicle_angular_velocity.h>

using namespace time_literals;
using namespace nextpilot::workq;
using namespace nextpilot;

/**
 * @brief 模拟IMU
 * 用ScheduleOnInterval()定周期发布角速率数据vehicle_angular_velocity
 *
 */
class SimIMU : public ModuleCommand<SimIMU>, public WorkItemScheduled {
public:
    SimIMU(int instance) :
        WorkItemScheduled(LOG_TAG, wq_configurations::lp_default),
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
        LOG_I("sim imu sensor initilized");
        ScheduleOnInterval(5_ms); // 200 Hz
        return 0;
    }

    void Run() override {
        hrt_abstime now_us        = hrt_absolute_time();
        _vangvel.timestamp        = now_us;
        _vangvel.timestamp_sample = now_us;
        for (int axis = 0; axis < 3; axis++) {
            _vangvel.xyz[axis]            = 1.0f;
            _vangvel.xyz_derivative[axis] = 0.0f;
        }
        _vangvel_pub.publish(_vangvel);


        _tick_buff[_tick_index % 50] = rt_tick_get();
        _tick_index++;
        //
        static int cnt = 0;
        if ((_instance == 0) && (cnt % 20 == 0)) {
            rt_tick_t current_tick = rt_tick_get();
            uint64_t  _start       = hrt_absolute_time();
            // LOG_I("[SimIMU]>>>instance=%d, t=%ld, dt=%ld, cnt=%d\n", _instance, current_tick, current_tick - _last_tick, cnt);

            uint64_t _delta = hrt_absolute_time() - _start;
            _last_tick      = current_tick;
        }
        cnt++;
    }

private:
    int       _instance{};
    rt_tick_t _last_tick{};
    rt_tick_t _tick_buff[50];
    int       _tick_index{0};

    //
    struct vehicle_angular_velocity_s _vangvel{};

    uORB::Publication<vehicle_angular_velocity_s> _vangvel_pub{ORB_ID(vehicle_angular_velocity)};
};

// 启动
int sim_imu_start() {
    int         ret;
    const char *argv[] = {"sim_imu", "start", "0"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    ret                = SimIMU::main(argc, (char **)argv);
    return ret;
}

// INIT_APP_EXPORT(sim_imu_start);
// MSH_CMD_EXPORT_ALIAS(sim_imu_start, utest_imu_sensor, sim imu rate sensor);

/**
 * @brief 命令入口函数
 * 启动: utest_imu_sensor start 0
 * 退出: utest_imu_sensor stop
 * 使用说明: utest_imu_sensor usage
 * 查看状态: utest_imu_sensor status
 *
 * @param argc
 * @param argv
 * @return int
 */
extern "C" int sim_imu_sensor_main(int argc, char *argv[]) {
    int ret;
    ret = SimIMU::main(argc, (char **)argv);

    return ret;
}

MSH_CMD_EXPORT_ALIAS(sim_imu_sensor_main, utest_imu_sensor, sim imu rate sensor);
