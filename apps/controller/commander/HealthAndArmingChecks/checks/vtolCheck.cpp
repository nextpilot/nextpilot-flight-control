/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "vtol_check"
#define LOG_LVL LOG_LVL_INFO

#include "vtolCheck.hpp"

using namespace time_literals;

void VtolChecks::checkAndReport(const Context &context, Report &reporter) {
    vtol_vehicle_status_s vtol_vehicle_status;

    if (_vtol_vehicle_status_sub.copy(&vtol_vehicle_status)) {
        reporter.failsafeFlags().vtol_fixed_wing_system_failure = vtol_vehicle_status.fixed_wing_system_failure;

        if (reporter.failsafeFlags().vtol_fixed_wing_system_failure) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system,
                                        events::ID("check_vtol_fixed_wing_system_failure"),
                                        events::Log::Error,
                                        "VTOL fixed-wing system failure detected. Verify reason for failure, and reboot the vehicle once confirmed safe");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_info(reporter.mavlink_log_pub(), "Preflight Fail: VTOL fixed-wing system failure detected\t");
            }
        }
    }
}
