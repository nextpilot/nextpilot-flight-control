/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/*
 * @file LandDetector.cpp
 *
 * @author Johan Jansen <jnsn.johan@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 */

#define LOG_TAG "land_detector"

#include "LandDetector.h"

using namespace time_literals;

namespace land_detector {

LandDetector::LandDetector() :
    ModuleParams(nullptr),
    WorkItemScheduled(MODULE_NAME, wq_configurations::nav_and_controllers) {
    _land_detected.ground_contact                   = true;
    _land_detected.maybe_landed                     = true;
    _land_detected.landed                           = true;
    _land_detected.in_ground_effect                 = true;
    _land_detected.in_descend                       = false;
    _land_detected.has_low_throttle                 = false;
    _land_detected.vertical_movement                = false;
    _land_detected.horizontal_movement              = false;
    _land_detected.rotational_movement              = false;
    _land_detected.close_to_ground_or_skipped_check = true;
    _land_detected.at_rest                          = true;
}

LandDetector::~LandDetector() {
    perf_free(_cycle_perf);
}

void LandDetector::start() {
    ScheduleDelayed(50_ms);
    _vehicle_local_position_sub.registerCallback();
}

void LandDetector::Run() {
    // push backup schedule
    ScheduleDelayed(50_ms);

    perf_begin(_cycle_perf);

    if (_parameter_update_sub.updated() || (_land_detected.timestamp == 0)) {
        parameter_update_s param_update;
        _parameter_update_sub.copy(&param_update);

        updateParams();
        _update_params();

        _total_flight_time = static_cast<uint64_t>(_param_total_flight_time_high.get()) << 32;
        _total_flight_time |= static_cast<uint32_t>(_param_total_flight_time_low.get());
    }

    actuator_armed_s actuator_armed;

    if (_actuator_armed_sub.update(&actuator_armed)) {
        _armed = actuator_armed.armed;
    }

    vehicle_acceleration_s vehicle_acceleration;

    if (_vehicle_acceleration_sub.update(&vehicle_acceleration)) {
        _acceleration = matrix::Vector3f{vehicle_acceleration.xyz};
    }

    vehicle_angular_velocity_s vehicle_angular_velocity{};

    if (_vehicle_angular_velocity_sub.update(&vehicle_angular_velocity)) {
        _angular_velocity = matrix::Vector3f{vehicle_angular_velocity.xyz};

        static constexpr float GYRO_NORM_MAX = math::radians(3.f); // 3 degrees/second

        if (_angular_velocity.norm() > GYRO_NORM_MAX) {
            _time_last_move_detect_us = vehicle_angular_velocity.timestamp_sample;
        }
    }

    _vehicle_local_position_sub.update(&_vehicle_local_position);
    _vehicle_status_sub.update(&_vehicle_status);

    _update_topics();

    if (!_dist_bottom_is_observable) {
        // we consider the distance to the ground observable if the system is using a range sensor
        _dist_bottom_is_observable = _vehicle_local_position.dist_bottom_sensor_bitfield &
                                     vehicle_local_position_s::DIST_BOTTOM_SENSOR_RANGE;
    }

    // Increase land detection time if not close to ground
    if (_dist_bottom_is_observable && !_vehicle_local_position.dist_bottom_valid) {
        _set_hysteresis_factor(3);

    } else {
        _set_hysteresis_factor(1);
    }

    const hrt_abstime now_us = hrt_absolute_time();

    _freefall_hysteresis.set_state_and_update(_get_freefall_state(), now_us);
    _ground_contact_hysteresis.set_state_and_update(_get_ground_contact_state(), now_us);
    _maybe_landed_hysteresis.set_state_and_update(_get_maybe_landed_state(), now_us);
    _landed_hysteresis.set_state_and_update(_get_landed_state(), now_us);
    _ground_effect_hysteresis.set_state_and_update(_get_ground_effect_state(), now_us);

    const bool freefallDetected       = _freefall_hysteresis.get_state();
    const bool ground_contactDetected = _ground_contact_hysteresis.get_state();
    const bool maybe_landedDetected   = _maybe_landed_hysteresis.get_state();
    const bool landDetected           = _landed_hysteresis.get_state();
    const bool in_ground_effect       = _ground_effect_hysteresis.get_state();

    UpdateVehicleAtRest();

    const bool at_rest = landDetected && _at_rest;

    // publish at 1 Hz, very first time, or when the result has changed
    if ((hrt_elapsed_time(&_land_detected.timestamp) >= 1_s) ||
        (_land_detected.landed != landDetected) ||
        (_land_detected.freefall != freefallDetected) ||
        (_land_detected.maybe_landed != maybe_landedDetected) ||
        (_land_detected.ground_contact != ground_contactDetected) ||
        (_land_detected.in_ground_effect != in_ground_effect) ||
        (_land_detected.at_rest != at_rest)) {
        if (!landDetected && _land_detected.landed && _takeoff_time == 0) { /* only set take off time once, until disarming */
            // We did take off
            _takeoff_time = now_us;
        }

        _land_detected.landed                           = landDetected;
        _land_detected.freefall                         = freefallDetected;
        _land_detected.maybe_landed                     = maybe_landedDetected;
        _land_detected.ground_contact                   = ground_contactDetected;
        _land_detected.in_ground_effect                 = in_ground_effect;
        _land_detected.in_descend                       = _get_in_descend();
        _land_detected.has_low_throttle                 = _get_has_low_throttle();
        _land_detected.horizontal_movement              = _get_horizontal_movement();
        _land_detected.vertical_movement                = _get_vertical_movement();
        _land_detected.rotational_movement              = _get_rotational_movement();
        _land_detected.close_to_ground_or_skipped_check = _get_close_to_ground_or_skipped_check();
        _land_detected.at_rest                          = at_rest;
        _land_detected.timestamp                        = hrt_absolute_time();
        _vehicle_land_detected_pub.publish(_land_detected);
    }

    // set the flight time when disarming (not necessarily when landed, because all param changes should
    // happen on the same event and it's better to set/save params while not in armed state)
    if (_takeoff_time != 0 && !_armed && _previous_armed_state) {
        _total_flight_time += now_us - _takeoff_time;
        _takeoff_time = 0;

        uint32_t flight_time = (_total_flight_time >> 32) & 0xffffffff;

        _param_total_flight_time_high.set(flight_time);
        _param_total_flight_time_high.commit_no_notification();

        flight_time = _total_flight_time & 0xffffffff;

        _param_total_flight_time_low.set(flight_time);
        _param_total_flight_time_low.commit_no_notification();
    }

    _previous_armed_state = _armed;

    perf_end(_cycle_perf);

    if (should_exit()) {
        ScheduleClear();
        exit_and_cleanup();
    }
}

void LandDetector::UpdateVehicleAtRest() {
    if (_sensor_selection_sub.updated()) {
        sensor_selection_s sensor_selection{};
        _sensor_selection_sub.copy(&sensor_selection);

        if (sensor_selection.gyro_device_id != _device_id_gyro) {
            bool gyro_status_found = false;

            // find corresponding vehicle_imu_status instance
            for (uint8_t imu_instance = 0; imu_instance < 4; imu_instance++) {
                uORB::Subscription imu_status_sub{ORB_ID(vehicle_imu_status), imu_instance};

                vehicle_imu_status_s imu_status{};
                imu_status_sub.copy(&imu_status);

                if ((imu_status.gyro_device_id != 0) && (imu_status.gyro_device_id == sensor_selection.gyro_device_id)) {
                    _vehicle_imu_status_sub.change_instance(imu_instance);
                    _device_id_gyro   = sensor_selection.gyro_device_id;
                    gyro_status_found = true;
                    break;
                }
            }

            if (!gyro_status_found) {
                PX4_WARN("IMU status not found for gyro %" PRId32, sensor_selection.gyro_device_id);
            }
        }
    }

    vehicle_imu_status_s imu_status;

    if (_vehicle_imu_status_sub.update(&imu_status)) {
        static constexpr float GYRO_VIBE_METRIC_MAX  = 0.02f; // gyro_vibration_metric * dt * 4.0e4f > is_moving_scaler)
        static constexpr float ACCEL_VIBE_METRIC_MAX = 1.2f;  // accel_vibration_metric * dt * 2.1e2f > is_moving_scaler

        if ((imu_status.gyro_vibration_metric > GYRO_VIBE_METRIC_MAX) || (imu_status.accel_vibration_metric > ACCEL_VIBE_METRIC_MAX)) {
            _time_last_move_detect_us = imu_status.timestamp;
        }
    }

    _at_rest = (hrt_elapsed_time(&_time_last_move_detect_us) > 1_s);
}

} // namespace land_detector
