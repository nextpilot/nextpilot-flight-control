/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "parachuteCheck.hpp"

using namespace time_literals;

void ParachuteChecks::checkAndReport(const Context &context, Report &reporter) {
    if (!_param_com_parachute.get()) {
        return;
    }

    if (!context.status().parachute_system_present) {
        /* EVENT
         * @description
         * Parachute system failed to report. Make sure it it setup and installed properly.
         *
         * <profile name="dev">
         * This check can be configured via <param>COM_PARACHUTE</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::parachute, events::ID("check_parachute_missing"),
                               events::Log::Error, "Parachute system missing");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Parachute system missing");
        }

    } else if (!context.status().parachute_system_healthy) {
        /* EVENT
         * @description
         * Parachute system reported being unhealth.
         *
         * <profile name="dev">
         * This check can be configured via <param>COM_PARACHUTE</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::parachute, events::ID("check_parachute_unhealthy"),
                               events::Log::Error, "Parachute system not ready");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Parachute system not ready");
        }
    }

    if (context.status().parachute_system_present) {
        reporter.setIsPresent(health_component_t::parachute);
    }
}
