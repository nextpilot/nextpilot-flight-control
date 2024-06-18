/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "ekf2_fake"
#define LOG_LVL LOG_LVL_INFO

#include "nextpilot.h"

#define EKF2_FAKE_PERIOD_MS 5

static bool    _ekf2_fake_thread_switch  = false;
static bool    _inject_gps_failsafe_flag = false;
static int32_t _sys_hitl                 = 0;

// // 订阅主题
// static orb_subsc_t sensor_gyro_sub;
// static orb_subsc_t v_acceleration_sub;
// static orb_subsc_t v_angular_velocity_sub;
// static orb_subsc_t v_attitude_sub;
// static orb_subsc_t v_local_position_sub;
// static orb_subsc_t v_global_position_sub;

// static rt_err_t ekf2_fake_init() {
//     sensor_gyro_sub        = orb_subscribe(ORB_ID(sensor_gyro));
//     v_acceleration_sub     = orb_subscribe(ORB_ID(sensor_accel));
//     v_angular_velocity_sub = orb_subscribe(ORB_ID(vehicle_angular_velocity_groundtruth));
//     v_attitude_sub         = orb_subscribe(ORB_ID(vehicle_attitude_groundtruth));
//     v_local_position_sub   = orb_subscribe(ORB_ID(vehicle_local_position_groundtruth));
//     v_global_position_sub  = orb_subscribe(ORB_ID(vehicle_global_position_groundtruth));
//     return RT_EOK;
// }

// static void ekf2_fake_step() {
//     hrt_abstime now_us = hrt_absolute_time();
//     if (_sys_hitl != 2) {
//         return;
//     }

//     // 发布 vehicle_angular_acceleration_s 主题
//     struct sensor_gyro_s                  sensor_gyro                  = {0}; // 订阅角速度主题，用于计算角加速度
//     struct vehicle_angular_acceleration_s vehicle_angular_acceleration = {0}; // 需要发布的主题
//     static struct sensor_gyro_s           last_senseor_gyro            = {0}; // 用于差分计算角加速度
//     if (orb_update(sensor_gyro_sub, &sensor_gyro) == RT_EOK) {
//         vehicle_angular_acceleration.timestamp        = now_us;
//         vehicle_angular_acceleration.timestamp_sample = 1;
//         vehicle_angular_acceleration.xyz[0]           = (sensor_gyro.x - last_senseor_gyro.x) / EKF2_FAKE_PERIOD_MS;
//         vehicle_angular_acceleration.xyz[1]           = (sensor_gyro.y - last_senseor_gyro.y) / EKF2_FAKE_PERIOD_MS;
//         vehicle_angular_acceleration.xyz[2]           = (sensor_gyro.z - last_senseor_gyro.z) / EKF2_FAKE_PERIOD_MS;
//         last_senseor_gyro                             = sensor_gyro;
//         orb_publish(ORB_ID(vehicle_angular_acceleration), NULL, &vehicle_angular_acceleration);
//     }

//     // 发布 vehicle_acceleration_s 主题
//     struct vehicle_acceleration_s v_acceleration = {0};
//     if (orb_update(v_acceleration_sub, &v_acceleration) == RT_EOK) {
//         struct vehicle_acceleration_s vehicle_acceleration = {0};
//         vehicle_acceleration.timestamp                     = now_us;
//         vehicle_acceleration.timestamp_sample              = v_acceleration.timestamp;
//         vehicle_acceleration.xyz[0]                        = v_acceleration.xyz[0];
//         vehicle_acceleration.xyz[1]                        = v_acceleration.xyz[1];
//         vehicle_acceleration.xyz[2]                        = v_acceleration.xyz[2];
//         orb_publish(ORB_ID(vehicle_acceleration), NULL, &vehicle_acceleration);
//     }

//     // 发布 vehicle_angular_velocity_s 主题
//     struct vehicle_angular_velocity_s angular_velocity_groundtruth = {0};
//     if (orb_update(v_angular_velocity_sub, &angular_velocity_groundtruth) == RT_EOK) {
//         struct vehicle_angular_velocity_s vehicle_angular_velocity = {0};
//         vehicle_angular_velocity.timestamp                         = now_us;
//         vehicle_angular_velocity.timestamp_sample                  = now_us;
//         vehicle_angular_velocity.xyz[0]                            = angular_velocity_groundtruth.xyz[0];
//         vehicle_angular_velocity.xyz[1]                            = angular_velocity_groundtruth.xyz[1];
//         vehicle_angular_velocity.xyz[2]                            = angular_velocity_groundtruth.xyz[2];
//         orb_publish(ORB_ID(vehicle_angular_velocity), NULL, &vehicle_angular_velocity);
//     }

//     // 发布 vehicle_attitude_s 主题
//     struct vehicle_attitude_s attitude_groundtruth = {0};
//     if (orb_update(v_attitude_sub, &attitude_groundtruth) == RT_EOK) {
//         struct vehicle_attitude_s attitude = {0};
//         attitude.timestamp                 = attitude_groundtruth.timestamp;
//         attitude.timestamp_sample          = attitude_groundtruth.timestamp_sample;
//         attitude.q[0]                      = attitude_groundtruth.q[0];
//         attitude.q[1]                      = attitude_groundtruth.q[1];
//         attitude.q[2]                      = attitude_groundtruth.q[2];
//         attitude.q[3]                      = attitude_groundtruth.q[3];
//         orb_publish(ORB_ID(vehicle_attitude), NULL, &attitude);
//     }

//     // 发布 local_position_s 主题
//     struct vehicle_local_position_s local_position_groundtruth = {0};
//     if (orb_update(v_local_position_sub, &local_position_groundtruth) == RT_EOK) {
//         struct vehicle_local_position_s local_position = {0};
//         if (!_inject_gps_failsafe_flag) {
//             rt_memcpy(&local_position, &local_position_groundtruth, sizeof(struct vehicle_local_position_s));
//         } else {
//             rt_memcpy(&local_position, &local_position_groundtruth, sizeof(struct vehicle_local_position_s));
//             local_position.eph        = 20.0f;
//             local_position.epv        = 20.0f;
//             local_position.evh        = 20.0f;
//             local_position.evv        = 20.0f;
//             local_position.xy_valid   = false;
//             local_position.v_xy_valid = false;
//             local_position.xy_global  = false;
//         }
//         orb_publish(ORB_ID(vehicle_local_position), NULL, &local_position);
//     }

//     // 发布 vehicle_global_position_s 主题
//     struct vehicle_global_position_s global_position_groundtruth = {0};
//     if (orb_update(v_global_position_sub, &global_position_groundtruth) == RT_EOK) {
//         struct vehicle_global_position_s global_position = {0};
//         if (!_inject_gps_failsafe_flag) {
//             rt_memcpy(&global_position, &global_position_groundtruth, sizeof(struct vehicle_global_position_s));
//         } else {
//             rt_memcpy(&global_position, &global_position_groundtruth, sizeof(struct vehicle_global_position_s));
//             global_position.eph            = 20.0f;
//             global_position.epv            = 20.0f;
//             global_position.dead_reckoning = true;
//         }
//         orb_publish(ORB_ID(vehicle_global_position), NULL, &global_position);
//     }
// }

static void ekf2_fake_run(void *parameter) {
    // ekf2_fake_init();

    while (_ekf2_fake_thread_switch) {
        // ekf2_fake_step();
        LOG_D("ISSSSS");
        rt_thread_mdelay(EKF2_FAKE_PERIOD_MS);
    }
}

static char             _ekf2_fake_thread_stack[1536];
static struct rt_thread _ekf2_fake_thread_handle;

static int ekf2_fake_start(void) {
    param_get(param_find("SYS_HITL"), &_sys_hitl);
    // TODO: param暂时不支持存储
    // SYS_HITL value 2 SIH enabled
    _sys_hitl = 2;
    if (_sys_hitl != 2) {
        return RT_EOK;
    }

    if (_ekf2_fake_thread_switch) {
        LOG_I("is running");
        return RT_EOK;
    }

    _ekf2_fake_thread_switch = true;

    rt_err_t result = rt_thread_init(&_ekf2_fake_thread_handle,
                                     "ekf2_fake",
                                     ekf2_fake_run,
                                     RT_NULL,
                                     &_ekf2_fake_thread_stack[0],
                                     sizeof(_ekf2_fake_thread_stack),
                                     10,
                                     5);

    if (result != RT_EOK) {
        LOG_E("thread init failed");
        return RT_ERROR;
    }

    result = rt_thread_startup(&_ekf2_fake_thread_handle);

    if (result != RT_EOK) {
        LOG_E("thread startup failed");
        return RT_ERROR;
    } else {
        LOG_I("start ok");
        return RT_EOK;
    }
}

INIT_APP_EXPORT(ekf2_fake_start);

/* Print useage */
static void print_help(void) {
    LOG_I("optional   arguments:  [start] [stop] [help] [status] [publish] [gf] [cgf] [usage]");
    LOG_I("ekf2_fake      start     --start module thread");
    LOG_I("               stop      --stop module thread");
    LOG_I("               help      --show help information");
    LOG_I("               status    --list module information status and statistics");
    LOG_I("               publish   --list module publish topics information");
    LOG_I("               gf        --inject gps failsafe");
    LOG_I("               cgf       --cancel gps failsafe");
    LOG_I("               usage     --list module usage");
}

static void ekf2_fake_stop(void) {
    if (!_ekf2_fake_thread_switch) {
        LOG_I("not running");

    } else {
        _ekf2_fake_thread_switch = false;
        LOG_I("stop the thread");
        LOG_I("_ekf2_fake_thread_switch: %d", _ekf2_fake_thread_switch);
    }
}

static void print_status(void) {
    LOG_I("To do");
}

static void print_publish(void) {
    LOG_I("To do");
}

static void inject_gps_failsafe(void) {
    _inject_gps_failsafe_flag = true;
    LOG_I("inject gps failsafe");
}

static void cancel_gps_failsafe(void) {
    _inject_gps_failsafe_flag = false;
    LOG_I("cancel gps failsafe");
}

static void print_usage(void) {
    LOG_I("This thread mainly creates ekf2 fake data for sitl simulation");
}

static int ekf2_fake_main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return RT_EOK;
    }

    if (!rt_strcmp(argv[1], "start")) {
        ekf2_fake_start();
    } else if (!rt_strcmp(argv[1], "stop")) {
        ekf2_fake_stop();
    } else if (!rt_strcmp(argv[1], "help")) {
        print_help();
    } else if (!rt_strcmp(argv[1], "status")) {
        print_status();
    } else if (!rt_strcmp(argv[1], "publish")) {
        print_publish();
    } else if (!rt_strcmp(argv[1], "gf")) {
        inject_gps_failsafe();
    } else if (!rt_strcmp(argv[1], "cgf")) {
        cancel_gps_failsafe();
    } else if (!rt_strcmp(argv[1], "usage")) {
        print_usage();
    } else {
        print_help();
    }

    return RT_EOK;
}

MSH_CMD_EXPORT_ALIAS(ekf2_fake_main, sim, create ekf2 fake date useage);
