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

    // publish vehicle_gyroscope_s
    if (_sensor_gyro_sub.update(&_s_gyro)) {
        vehicle_angular_velocity_s        vehicle_angular_velocity{};
        static vehicle_angular_velocity_s vehicle_angular_velocity_prev{};
        vehicle_angular_velocity.timestamp        = now_us;
        vehicle_angular_velocity.timestamp_sample = vehicle_angular_velocity.timestamp;
        vehicle_angular_velocity.xyz[0]           = _s_gyro.x;
        vehicle_angular_velocity.xyz[1]           = _s_gyro.y;
        vehicle_angular_velocity.xyz[2]           = _s_gyro.z;
        for (int axis = 0; axis < 3; axis++) {
            vehicle_angular_velocity.xyz_derivative[axis] =
                (vehicle_angular_velocity.xyz[axis] - vehicle_angular_velocity_prev.xyz[axis])
                / (EKF2_FAKE_PERIOD_MS / 1000.0f);
        }
        vehicle_angular_velocity_prev = vehicle_angular_velocity;
        _v_ang_pub.publish(vehicle_angular_velocity);
    }

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

    // publish vehicle gps position
    if (_sensor_gps_sub.update(&_s_gps)) {
        sensor_gps_s vehicle_gps_pos{};
        vehicle_gps_pos = _s_gps;
        _v_gps_pos_pub.publish(vehicle_gps_pos);
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

    // publish estimator status
    _esttimator_status.timestamp        = now_us;
    _esttimator_status.timestamp_sample = now_us;
    // uint64_t control_mode_flags; // TODO:
    for (int i = 0; i < 3; i++) {
        _esttimator_status.output_tracking_error[i] = 0;
    }
    _esttimator_status.filter_fault_flags               = 0;
    _esttimator_status.pos_horiz_accuracy               = 0.1f;
    _esttimator_status.pos_vert_accuracy                = 0.1f;
    _esttimator_status.mag_test_ratio                   = 0.1f;
    _esttimator_status.vel_test_ratio                   = 0.1f;
    _esttimator_status.pos_test_ratio                   = 0.1f;
    _esttimator_status.hgt_test_ratio                   = 0.1f;
    _esttimator_status.tas_test_ratio                   = 0.1f;
    _esttimator_status.hagl_test_ratio                  = 0.1f;
    _esttimator_status.beta_test_ratio                  = 0.1f;
    _esttimator_status.time_slip                        = 0;
    _esttimator_status.accel_device_id                  = 0xFF;
    _esttimator_status.gyro_device_id                   = 0xFF;
    _esttimator_status.baro_device_id                   = 0xFF;
    _esttimator_status.mag_device_id                    = 0xFF;
    _esttimator_status.gps_check_fail_flags             = 0;
    _esttimator_status.innovation_check_flags           = 0;
    _esttimator_status.solution_status_flags            = 0;
    _esttimator_status.reset_count_vel_ne               = 0;
    _esttimator_status.reset_count_vel_d                = 0;
    _esttimator_status.reset_count_pos_ne               = 0;
    _esttimator_status.reset_count_pod_d                = 0;
    _esttimator_status.reset_count_quat                 = 0;
    _esttimator_status.pre_flt_fail_innov_heading       = false;
    _esttimator_status.pre_flt_fail_innov_vel_horiz     = false;
    _esttimator_status.pre_flt_fail_innov_vel_vert      = false;
    _esttimator_status.pre_flt_fail_innov_height        = false;
    _esttimator_status.pre_flt_fail_mag_field_disturbed = false;
    _esttimator_status.health_flags                     = 0;
    _esttimator_status.timeout_flags                    = 0;
    _estimator_status_pub.publish(_esttimator_status);
}
