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

EKF2FAKE::EKF2FAKE() {
}

EKF2FAKE::~EKF2FAKE() {
}

int EKF2FAKE::init() {
    return 1;
}

void EKF2FAKE::publish_imu_data(const hrt_abstime now_us) {
    if (_vangvel_gt_sub.update(&_vangvel_gt)) {
        _vangvel.timestamp        = now_us;
        _vangvel.timestamp_sample = now_us;
        for (int axis = 0; axis < 3; axis++) {
            _vangvel.xyz[axis]            = _vangvel_gt.xyz[axis];
            _vangvel.xyz_derivative[axis] = _vangvel_gt.xyz_derivative[axis];
        }
        _vangvel_pub.publish(_vangvel);
    }
    if (_sacc_sub.update(&_sacc)) {
        _vacc.timestamp        = now_us;
        _vacc.timestamp_sample = _sacc.timestamp;
        _vacc.xyz[0]           = _sacc.x;
        _vacc.xyz[1]           = _sacc.y;
        _vacc.xyz[2]           = _sacc.z;
        _vacc_pub.publish(_vacc);
    }
}

void EKF2FAKE::publish_gps_data(const hrt_abstime now_us) {
    if (_sgps_sub.update(&_sgps)) {
        rt_memcpy(&_vgpspos, &_sgps, sizeof(sensor_gps_s));
        _vgpspos_pub.publish(_vgpspos);
    }
}

void EKF2FAKE::publish_ekf_data(const hrt_abstime now_us) {
    if (_vatt_gt_sub.update(&_vatt_gt)) {
        rt_memcpy(&_vatt, &_vatt_gt, sizeof(vehicle_attitude_s));
        _vatt_pub.publish(_vatt);
    }
    if (_vlpos_gt_sub.update(&_vlpos_gt)) {
        rt_memcpy(&_vlpos, &_vlpos_gt, sizeof(vehicle_local_position_s));
        _vlpos_pub.publish(_vlpos);
    }
    if (_vgpos_gt_sub.update(&_vgpos_gt)) {
        rt_memcpy(&_vgpos, &_vgpos_gt, sizeof(vehicle_global_position_s));
        _vgpos_pub.publish(_vgpos);
    }
}

void EKF2FAKE::publish_est_data(const hrt_abstime now_us) {
    _est_status.timestamp        = now_us;
    _est_status.timestamp_sample = now_us;
    // uint64_t control_mode_flags; // TODO:
    for (int i = 0; i < 3; i++) {
        _est_status.output_tracking_error[i] = 0;
    }
    _est_status.filter_fault_flags               = 0;
    _est_status.pos_horiz_accuracy               = 0.1f;
    _est_status.pos_vert_accuracy                = 0.1f;
    _est_status.mag_test_ratio                   = 0.1f;
    _est_status.vel_test_ratio                   = 0.1f;
    _est_status.pos_test_ratio                   = 0.1f;
    _est_status.hgt_test_ratio                   = 0.1f;
    _est_status.tas_test_ratio                   = 0.1f;
    _est_status.hagl_test_ratio                  = 0.1f;
    _est_status.beta_test_ratio                  = 0.1f;
    _est_status.time_slip                        = 0;
    _est_status.accel_device_id                  = 42688;
    _est_status.gyro_device_id                   = 42688;
    _est_status.baro_device_id                   = 5611;
    _est_status.mag_device_id                    = 8310;
    _est_status.gps_check_fail_flags             = 0;
    _est_status.innovation_check_flags           = 0;
    _est_status.solution_status_flags            = 0;
    _est_status.reset_count_vel_ne               = 0;
    _est_status.reset_count_vel_d                = 0;
    _est_status.reset_count_pos_ne               = 0;
    _est_status.reset_count_pod_d                = 0;
    _est_status.reset_count_quat                 = 0;
    _est_status.pre_flt_fail_innov_heading       = false;
    _est_status.pre_flt_fail_innov_vel_horiz     = false;
    _est_status.pre_flt_fail_innov_vel_vert      = false;
    _est_status.pre_flt_fail_innov_height        = false;
    _est_status.pre_flt_fail_mag_field_disturbed = false;
    _est_status.health_flags                     = 0;
    _est_status.timeout_flags                    = 0;
    _est_status_pub.publish(_est_status);
}

#ifdef USING_COMMANDER_FAKE
void EKF2FAKE::publish_cmd_data(const hrt_abstime now_us) {
    _armed.timestamp               = now_us;
    _armed.armed                   = true;
    _armed.prearmed                = false;
    _armed.ready_to_arm            = false;
    _armed.lockdown                = false;
    _armed.manual_lockdown         = false;
    _armed.force_failsafe          = false;
    _armed.in_esc_calibration_mode = false;
    _armed_pub.publish(_armed);

    // _actuator_test{};
    // _actuator_test_pub.publish(_actuator_test);
    // _fd_status{};
    // _fd_status_pub.publish(_fd_status);
    // _vcmd_ack{};
    // _vcmd_ack_pub.publish(_vcmd_ack);

    // For ACRO mode
    _vctr_mode.timestamp                                 = now_us;
    _vctr_mode.flag_armed                                = true;
    _vctr_mode.flag_multicopter_position_control_enabled = true;
    _vctr_mode.flag_control_manual_enabled               = true;
    _vctr_mode.flag_control_auto_enabled                 = true;
    _vctr_mode.flag_control_offboard_enabled             = true;
    _vctr_mode.flag_control_rates_enabled                = true;
    _vctr_mode.flag_control_attitude_enabled             = false;
    _vctr_mode.flag_control_acceleration_enabled         = true;
    _vctr_mode.flag_control_velocity_enabled             = true;
    _vctr_mode.flag_control_position_enabled             = true;
    _vctr_mode.flag_control_altitude_enabled             = true;
    _vctr_mode.flag_control_climb_rate_enabled           = true;
    _vctr_mode.flag_control_termination_enabled          = true;
    _vctr_mode_pub.publish(_vctr_mode);

    _vstatus.timestamp                    = now_us;
    _vstatus.armed_time                   = now_us;
    _vstatus.takeoff_time                 = now_us;
    _vstatus.nav_state_timestamp          = now_us;
    _vstatus.failure_detector_status      = 0;
    _vstatus.arming_state                 = 0;
    _vstatus.latest_arming_reason         = 0;
    _vstatus.latest_disarming_reason      = 0;
    _vstatus.nav_state_user_intention     = 0;
    _vstatus.nav_state                    = 0;
    _vstatus.hil_state                    = vehicle_status_s::HIL_STATE_ON;
    _vstatus.vehicle_type                 = vehicle_status_s::VEHICLE_TYPE_ROTARY_WING;
    _vstatus.failsafe                     = 0;
    _vstatus.failsafe_and_user_took_over  = 0;
    _vstatus.gcs_connection_lost          = 0;
    _vstatus.gcs_connection_lost_counter  = 0;
    _vstatus.high_latency_data_link_lost  = 0;
    _vstatus.is_vtol                      = false;
    _vstatus.is_vtol_tailsitter           = false;
    _vstatus.in_transition_mode           = false;
    _vstatus.in_transition_to_fw          = false;
    _vstatus.system_type                  = 0;
    _vstatus.system_id                    = 0;
    _vstatus.component_id                 = 0;
    _vstatus.safety_button_available      = 0;
    _vstatus.safety_off                   = 0;
    _vstatus.power_input_valid            = 0;
    _vstatus.usb_connected                = 0;
    _vstatus.open_drone_id_system_present = 0;
    _vstatus.open_drone_id_system_healthy = 0;
    _vstatus.parachute_system_present     = 0;
    _vstatus.parachute_system_healthy     = 0;
    _vstatus.avoidance_system_required    = 0;
    _vstatus.avoidance_system_valid       = 0;
    _vstatus.rc_calibration_in_progress   = 0;
    _vstatus.calibration_enabled          = 0;
    _vstatus.pre_flight_checks_pass       = 0;
    _vstatus_pub.publish(_vstatus);
}
#endif // USING_COMMANDER_FAKE

void EKF2FAKE::Run() {
    hrt_abstime current_us = hrt_absolute_time();
    publish_imu_data(current_us);
    publish_gps_data(current_us);
    publish_ekf_data(current_us);
    publish_est_data(current_us);
    // just for test
#ifdef USING_COMMANDER_FAKE
    publish_cmd_data(current_us);
#endif // USING_COMMANDER_FAKE
}
