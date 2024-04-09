/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "accelerometerCheck.hpp"

#include <lib/sensor_calibration/Utilities.hpp>

using namespace time_literals;

void AccelerometerChecks::checkAndReport(const Context &context, Report &reporter) {
    for (int instance = 0; instance < _sensor_accel_sub.size(); instance++) {
        const bool is_required = instance == 0 || isAccelRequired(instance);

        if (!is_required) {
            continue;
        }

        const bool exists               = _sensor_accel_sub[instance].advertised();
        bool       is_valid             = false;
        bool       is_calibration_valid = false;

        if (exists) {
            sensor_accel_s accel_data;
            is_valid = _sensor_accel_sub[instance].copy(&accel_data) && (accel_data.device_id != 0) && (accel_data.timestamp != 0) && (hrt_elapsed_time(&accel_data.timestamp) < 1_s);

            if (context.status().hil_state == vehicle_status_s::HIL_STATE_ON) {
                is_calibration_valid = true;

            } else {
                is_calibration_valid = (calibration::FindCurrentCalibrationIndex("ACC", accel_data.device_id) >= 0);
            }

            reporter.setIsPresent(health_component_t::gyro);
        }

        const bool is_sensor_ok = is_valid && is_calibration_valid;

        if (!is_sensor_ok) {
            if (!exists) {
                /* EVENT
                 */
                reporter.healthFailure<uint8_t>(NavModes::All, health_component_t::accel, events::ID("check_accel_missing"),
                                                events::Log::Error, "Accelerometer sensor {1} missing", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Accel Sensor %u missing", instance);
                }

            } else if (!is_valid) {
                /* EVENT
                 */
                reporter.healthFailure<uint8_t>(NavModes::All, health_component_t::accel, events::ID("check_accel_no_data"),
                                                events::Log::Error, "No valid data from Accelerometer {1}", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: No valid data from Accel %u", instance);
                }

            } else if (!is_calibration_valid) {
                /* EVENT
                 */
                reporter.armingCheckFailure<uint8_t>(NavModes::All, health_component_t::accel, events::ID("check_accel_not_calibrated"),
                                                     events::Log::Error, "Accelerometer {1} uncalibrated", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Accel %u uncalibrated", instance);
                }
            }
        }
    }
}

bool AccelerometerChecks::isAccelRequired(int instance) {
    sensor_accel_s sensor_accel;

    if (!_sensor_accel_sub[instance].copy(&sensor_accel)) {
        return false;
    }

    const uint32_t device_id = static_cast<uint32_t>(sensor_accel.device_id);

    if (device_id == 0) {
        return false;
    }

    bool is_used_by_nav = false;

    for (int i = 0; i < _estimator_status_sub.size(); i++) {
        estimator_status_s estimator_status;

        if (_estimator_status_sub[i].copy(&estimator_status) && estimator_status.accel_device_id == device_id) {
            is_used_by_nav = true;
            break;
        }
    }

    return is_used_by_nav;
}
