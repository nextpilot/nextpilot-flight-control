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

#include "ekf2_fake.hpp"

Ekf2Fake::Ekf2Fake() {
}

Ekf2Fake::~Ekf2Fake() {
}

int Ekf2Fake::init() {
    return 0;
}

void Ekf2Fake::Run() {
    hrt_abstime now_us = hrt_absolute_time();
    LOG_D("is running");

    // publish vehicle_acceleration_s
    if (_sensor_acc_sub.update(&_s_accel)) {
        vehicle_acceleration_s vehicle_acceleration{};
        vehicle_acceleration.timestamp        = now_us;
        vehicle_acceleration.timestamp_sample = _s_accel.timestamp;
        vehicle_acceleration.xyz[0]           = _s_accel.x;
        vehicle_acceleration.xyz[1]           = _s_accel.y;
        vehicle_acceleration.xyz[2]           = _s_accel.z;
        _v_acc_pub.publish(vehicle_acceleration);
    }

    // publish vehicle angular velocity
    if (_v_ang_vel_groundtruth_sub.update(&_v_ang_vel_groundtruth)) {
        vehicle_angular_velocity_s vehicle_angular_velocity{};
        vehicle_angular_velocity.timestamp        = now_us;
        vehicle_angular_velocity.timestamp_sample = now_us;
        for (int axis = 0; axis < 3; axis++) {
            vehicle_angular_velocity.xyz[axis]            = _v_ang_vel_groundtruth.xyz[axis];
            vehicle_angular_velocity.xyz_derivative[axis] = _v_ang_vel_groundtruth.xyz_derivative[axis];
        }
        _v_ang_vel_pub.publish(vehicle_angular_velocity);
    }

    // publish vehicle attitude
    if (_v_att_groundtruth_sub.update(&_v_att_groundtruth)) {
        vehicle_attitude_s attitude{};
        attitude.timestamp          = _v_att_groundtruth.timestamp;
        attitude.timestamp_sample   = _v_att_groundtruth.timestamp_sample;
        attitude.quat_reset_counter = _v_att_groundtruth.quat_reset_counter;
        for (int i = 0; i < 4; i++) {
            attitude.q[i]             = _v_att_groundtruth.q[i];
            attitude.delta_q_reset[i] = _v_att_groundtruth.delta_q_reset[i];
        }
        _v_att_pub.publish(attitude);
    }

    // publish local_position
    if (_v_loc_pos_groundtruth_sub.update(&_v_loc_pos_groundtruth)) {
        vehicle_local_position_s local_position{};
        rt_memcpy(&local_position, &_v_loc_pos_groundtruth, sizeof(vehicle_local_position_s));
        _v_loc_pos_pub.publish(local_position);
    }

    // publish vehicle global position
    if (_v_glob_pos_groundtruth_sub.update(&_v_glob_pos_groundtruth)) {
        vehicle_global_position_s global_position{};
        rt_memcpy(&global_position, &_v_glob_pos_groundtruth, sizeof(vehicle_global_position_s));
        _v_glob_pos_pub.publish(global_position);
    }
}
