/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "baroCheck.hpp"

using namespace time_literals;

void BaroChecks::checkAndReport(const Context &context, Report &reporter) {
    if (!_param_sys_has_baro.get()) {
        return;
    }

    for (int instance = 0; instance < _sensor_baro_sub.size(); instance++) {
        const bool is_required = instance == 0 || isBaroRequired(instance);

        if (!is_required) {
            continue;
        }

        const bool exists   = _sensor_baro_sub[instance].advertised();
        bool       is_valid = false;

        if (exists) {
            sensor_baro_s baro_data;
            is_valid = _sensor_baro_sub[instance].copy(&baro_data) && (baro_data.device_id != 0) && (baro_data.timestamp != 0) && (hrt_elapsed_time(&baro_data.timestamp) < 1_s);
            reporter.setIsPresent(health_component_t::absolute_pressure);
        }

        const bool is_sensor_ok = is_valid;

        if (!is_sensor_ok) {
            if (!exists) {
                /* EVENT
                 */
                reporter.healthFailure<uint8_t>(NavModes::All, health_component_t::absolute_pressure,
                                                events::ID("check_baro_missing"),
                                                events::Log::Error, "Barometer {1} missing", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: barometer %u missing", instance);
                }

            } else if (!is_valid) {
                /* EVENT
                 */
                reporter.healthFailure<uint8_t>(NavModes::All, health_component_t::absolute_pressure,
                                                events::ID("check_baro_no_data"),
                                                events::Log::Error, "No valid data from barometer {1}", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: No valid data from Baro %u", instance);
                }
            }
        }
    }
}

bool BaroChecks::isBaroRequired(int instance) {
    sensor_baro_s sensor_baro;

    if (!_sensor_baro_sub[instance].copy(&sensor_baro)) {
        return false;
    }

    const uint32_t device_id = static_cast<uint32_t>(sensor_baro.device_id);

    if (device_id == 0) {
        return false;
    }

    bool is_used_by_nav = false;

    for (int i = 0; i < _estimator_status_sub.size(); i++) {
        estimator_status_s estimator_status;

        if (_estimator_status_sub[i].copy(&estimator_status) && estimator_status.baro_device_id == device_id) {
            is_used_by_nav = true;
            break;
        }
    }

    return is_used_by_nav;
}
