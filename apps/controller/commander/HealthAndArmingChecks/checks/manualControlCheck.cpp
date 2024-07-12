/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "rc_check"
#define LOG_LVL LOG_LVL_INFO

#include "manualControlCheck.hpp"

using namespace time_literals;

void ManualControlChecks::checkAndReport(const Context &context, Report &reporter) {
    if (context.isArmed()) {
        return;
    }

    manual_control_switches_s manual_control_switches;

    if (_manual_control_switches_sub.copy(&manual_control_switches)) {
        // check action switches
        if (manual_control_switches.return_switch == manual_control_switches_s::SWITCH_POS_ON) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::remote_control,
                                        events::ID("check_man_control_rtl_engaged"),
                                        events::Log::Error, "RTL switch engaged");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: RTL switch engaged");
            }
        }

        if (manual_control_switches.kill_switch == manual_control_switches_s::SWITCH_POS_ON) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::remote_control,
                                        events::ID("check_man_control_kill_engaged"),
                                        events::Log::Error, "Kill switch engaged");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Kill switch engaged");
            }
        }

        if (manual_control_switches.gear_switch == manual_control_switches_s::SWITCH_POS_ON) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::remote_control,
                                        events::ID("check_man_control_landing_gear_up"),
                                        events::Log::Error, "Landing gear switch set in UP position");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Landing gear switch set in UP position");
            }
        }
    }
}
