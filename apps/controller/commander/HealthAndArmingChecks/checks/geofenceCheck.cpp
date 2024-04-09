/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "geofenceCheck.hpp"

void GeofenceChecks::checkAndReport(const Context &context, Report &reporter) {
    geofence_result_s geofence_result;

    if (!_geofence_result_sub.copy(&geofence_result)) {
        geofence_result = {};
    }

    reporter.failsafeFlags().primary_geofence_breached = geofence_result.primary_geofence_breached;

    if (geofence_result.primary_geofence_action != 0 && reporter.failsafeFlags().primary_geofence_breached) {
        /* EVENT
         * @description
         * <profile name="dev">
         * This check can be configured via <param>GF_ACTION</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure<events::px4::enums::geofence_violation_reason_t>(NavModes::All, health_component_t::system,
                                                                                     events::ID("check_gf_violation"),
                                                                                     events::Log::Error, "Geofence violation: {1}",
                                                                                     (events::px4::enums::geofence_violation_reason_t)geofence_result.geofence_violation_reason);

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Geofence violation");
        }
    }

    if (geofence_result.primary_geofence_action == geofence_result_s::GF_ACTION_RTL && reporter.failsafeFlags().home_position_invalid) {
        /* EVENT
         * @description
         * <profile name="dev">
         * This check can be configured via <param>GF_ACTION</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_gf_no_home"),
                                    events::Log::Error, "Geofence RTL requires valid home");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Geofence RTL requires valid home");
        }
    }
}
