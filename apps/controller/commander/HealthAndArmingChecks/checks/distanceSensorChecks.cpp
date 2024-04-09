/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "distanceSensorChecks.hpp"

using namespace time_literals;

void DistanceSensorChecks::checkAndReport(const Context &context, Report &reporter) {
    if (_param_sys_has_num_dist.get() <= 0) {
        return;
    }

    for (int instance = 0; instance < _distance_sensor_sub.size(); instance++) {
        const bool exists       = _distance_sensor_sub[instance].advertised();
        const bool is_mandatory = instance < _param_sys_has_num_dist.get();
        bool       valid        = false;

        if (exists) {
            distance_sensor_s dist_sens;
            valid = _distance_sensor_sub[instance].copy(&dist_sens) && hrt_elapsed_time(&dist_sens.timestamp) < 1_s;
            reporter.setIsPresent(health_component_t::distance_sensor);
        }

        if (is_mandatory) {
            if (!exists) {
                /* EVENT
                 * @description
                 * <profile name="dev">
                 * This check can be configured via <param>SYS_HAS_NUM_DIST</param> parameter.
                 * </profile>
                 */
                reporter.healthFailure<uint8_t>(NavModes::All, health_component_t::distance_sensor,
                                                events::ID("check_distance_sensor_missing"),
                                                events::Log::Error, "Distance sensor {1} missing", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Distance Sensor %u missing", instance);
                }

            } else if (!valid) {
                /* EVENT
                 */
                reporter.healthFailure<uint8_t>(NavModes::All, health_component_t::distance_sensor,
                                                events::ID("check_distance_sensor_invalid"),
                                                events::Log::Error, "No valid data from distance sensor {1}", instance);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: no valid data from distance sensor %u", instance);
                }
            }
        }
    }
}
