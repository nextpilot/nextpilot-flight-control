/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "mission_check"
#define LOG_LVL LOG_LVL_INFO

#include "missionCheck.hpp"

void MissionChecks::checkAndReport(const Context &context, Report &reporter) {
    reporter.failsafeFlags().mission_failure      = false;
    reporter.failsafeFlags().auto_mission_missing = true;
    mission_result_s mission_result;

    if (_mission_result_sub.copy(&mission_result) && mission_result.valid) {
        reporter.failsafeFlags().mission_failure = mission_result.failure;

        if (reporter.failsafeFlags().mission_failure) {
            // navigator sends out the exact reason
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system,
                                        events::ID("check_mission_failure"),
                                        events::Log::Error, "Mission cannot be completed");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Mission cannot be completed\t");
            }
        }

        // This is a mode requirement, no need to report
        reporter.failsafeFlags().auto_mission_missing = mission_result.instance_count <= 0;
    }
}
