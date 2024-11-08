/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "rcAndDataLinkCheck.hpp"

using namespace time_literals;

void RcAndDataLinkChecks::checkAndReport(const Context &context, Report &reporter) {
    // RC
    manual_control_setpoint_s manual_control_setpoint;

    if (!_manual_control_setpoint_sub.copy(&manual_control_setpoint)) {
        manual_control_setpoint                             = {};
        reporter.failsafeFlags().manual_control_signal_lost = true;
    }

    // Check if RC is valid
    if (!manual_control_setpoint.valid || hrt_elapsed_time(&manual_control_setpoint.timestamp) > _param_com_rc_loss_t.get() * 1_s) {
        if (!reporter.failsafeFlags().manual_control_signal_lost && _last_valid_manual_control_setpoint > 0) {
            events::send(events::ID("commander_rc_lost"), {events::Log::Critical, events::LogInternal::Info},
                         "Manual control lost");
        }

        reporter.failsafeFlags().manual_control_signal_lost = true;

    } else {
        reporter.setIsPresent(health_component_t::remote_control);

        if (reporter.failsafeFlags().manual_control_signal_lost && _last_valid_manual_control_setpoint > 0) {
            float elapsed = hrt_elapsed_time(&_last_valid_manual_control_setpoint) * 1e-6f;
            events::send<float>(events::ID("commander_rc_regained"), events::Log::Info,
                                "Manual control regained after {1:.1} s", elapsed);
        }

        reporter.failsafeFlags().manual_control_signal_lost = false;
        _last_valid_manual_control_setpoint                 = manual_control_setpoint.timestamp;
    }

    // Manual control check is in modeCheck as mode requirement

    // GCS connection
    reporter.failsafeFlags().gcs_connection_lost = context.status().gcs_connection_lost;

    if (reporter.failsafeFlags().gcs_connection_lost) {
        // Prevent arming if we neither have RC nor a GCS connection. TODO: disabled for now due to MAVROS tests
        bool gcs_connection_required = _param_nav_dll_act.get() > 0
            /*|| (rc_is_optional && reporter.failsafeFlags().manual_control_signal_lost) */;
        NavModes         affected_modes = gcs_connection_required ? NavModes::All : NavModes::None;
        events::LogLevel log_level      = gcs_connection_required ? events::Log::Error : events::Log::Info;
        /* EVENT
         * @description
         * To arm, at least a data link or manual control (RC) must be present.
         *
         * <profile name="dev">
         * This check can be configured via <param>NAV_DLL_ACT</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(affected_modes, health_component_t::communication_links,
                                    events::ID("check_rc_dl_no_dllink"),
                                    log_level, "No connection to the ground control station");

        if (gcs_connection_required && reporter.mavlink_log_pub()) {
            mavlink_log_warning(reporter.mavlink_log_pub(), "Preflight Fail: No connection to the ground control station\t");
        }

    } else {
        reporter.setIsPresent(health_component_t::communication_links);
    }
}
