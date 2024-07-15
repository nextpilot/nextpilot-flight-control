/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "mode_check"
#define LOG_LVL LOG_LVL_INFO

#include "modeCheck.hpp"

void ModeChecks::checkAndReport(const Context &context, Report &reporter) {
    if (!context.isArmed()) {
        checkArmingRequirement(context, reporter);
    }

    // Failing mode requirements generally also clear the can_run bits which prevents mode switching and
    // might trigger a failsafe if already in that mode.

    if (reporter.failsafeFlags().angular_velocity_invalid && reporter.failsafeFlags().mode_req_angular_velocity != 0) {
        /* EVENT
         * @description
         * Make sure the gyroscope is providing valid data.
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_angular_velocity, health_component_t::system,
                                    events::ID("check_modes_angular_velocity"),
                                    events::Log::Critical, "Angular velocity not valid");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_angular_velocity);
    }

    if (reporter.failsafeFlags().attitude_invalid && reporter.failsafeFlags().mode_req_attitude != 0) {
        /* EVENT
         * @description
         * Wait until the estimator initialized
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_attitude, health_component_t::system,
                                    events::ID("check_modes_attitude"),
                                    events::Log::Critical, "No valid attitude estimate");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_attitude);
    }

    NavModes local_position_modes = NavModes::None;

    if (reporter.failsafeFlags().local_position_invalid && reporter.failsafeFlags().mode_req_local_position != 0) {
        local_position_modes = (NavModes)reporter.failsafeFlags().mode_req_local_position;
    }

    if (reporter.failsafeFlags().local_position_invalid_relaxed && reporter.failsafeFlags().mode_req_local_position_relaxed != 0) {
        local_position_modes = local_position_modes | (NavModes)reporter.failsafeFlags().mode_req_local_position_relaxed;
    }

    if (local_position_modes != NavModes::None) {
        /* EVENT
         */
        reporter.armingCheckFailure(local_position_modes, health_component_t::system,
                                    events::ID("check_modes_local_pos"),
                                    events::Log::Error, "No valid local position estimate");
        reporter.clearCanRunBits(local_position_modes);
    }

    if (reporter.failsafeFlags().global_position_invalid && reporter.failsafeFlags().mode_req_global_position != 0) {
        /* EVENT
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_global_position, health_component_t::system,
                                    events::ID("check_modes_global_pos"),
                                    events::Log::Error, "No valid global position estimate");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_global_position);
    }

    if (reporter.failsafeFlags().local_altitude_invalid && reporter.failsafeFlags().mode_req_local_alt != 0) {
        /* EVENT
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_local_alt, health_component_t::system,
                                    events::ID("check_modes_local_alt"),
                                    events::Log::Critical, "No valid altitude estimate");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_local_alt);
    }

    NavModes mission_required_modes = (NavModes)reporter.failsafeFlags().mode_req_mission;

    if (_param_com_arm_mis_req.get()) {
        mission_required_modes = NavModes::All;
    }

    if (reporter.failsafeFlags().auto_mission_missing && mission_required_modes != NavModes::None) {
        /* EVENT
         * @description
         * Upload a mission first.
         *
         * <profile name="dev">
         * This check can be configured via <param>COM_ARM_MIS_REQ</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(mission_required_modes, health_component_t::system,
                                    events::ID("check_modes_mission"),
                                    events::Log::Info, "No valid mission available");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_mission);
    }

    if (reporter.failsafeFlags().offboard_control_signal_lost && reporter.failsafeFlags().mode_req_offboard_signal != 0) {
        /* EVENT
         * @description
         * The offboard component is not sending setpoints or the required estimate (e.g. position) is missing.
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_offboard_signal, health_component_t::system,
                                    events::ID("check_modes_offboard_signal"),
                                    events::Log::Error, "No offboard signal");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_offboard_signal);
    }

    if (reporter.failsafeFlags().home_position_invalid && reporter.failsafeFlags().mode_req_home_position != 0) {
        /* EVENT
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_home_position, health_component_t::system,
                                    events::ID("check_modes_home_position"),
                                    events::Log::Info, "Home position not set");
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_home_position);
    }

    if (reporter.failsafeFlags().manual_control_signal_lost && reporter.failsafeFlags().mode_req_manual_control != 0) {
        const bool       rc_disabled = (_param_com_rc_in_mode.get() == 4);
        NavModes         nav_modes   = rc_disabled ? (NavModes)reporter.failsafeFlags().mode_req_manual_control : NavModes::None;
        events::LogLevel log_level   = rc_disabled ? events::Log::Error : events::Log::Warning;

        /* EVENT
         * @description
         * Connect and enable stick input or use autonomous mode.
         * <profile name="dev">
         * Sticks can be enabled via <param>COM_RC_IN_MODE</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(nav_modes,
                                    health_component_t::remote_control,
                                    events::ID("check_modes_manual_control"),
                                    log_level, "No manual control input");
        reporter.clearArmingBits((NavModes)reporter.failsafeFlags().mode_req_manual_control);
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_manual_control);
    }

    if (reporter.failsafeFlags().mode_req_other != 0) {
        // Here we expect there is already an event reported for the failing check (this is for external modes)
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_other);
    }

    if ((reporter.failsafeFlags().flight_time_limit_exceeded || reporter.failsafeFlags().wind_limit_exceeded) && reporter.failsafeFlags().mode_req_wind_and_flight_time_compliance != 0) {
        // Already reported
        reporter.clearCanRunBits((NavModes)reporter.failsafeFlags().mode_req_wind_and_flight_time_compliance);
    }
}

void ModeChecks::checkArmingRequirement(const Context &context, Report &reporter) {
    if (reporter.failsafeFlags().mode_req_prevent_arming & (1u << context.status().nav_state)) {
        /* EVENT
         * @description
         * Switch to another mode first.
         */
        reporter.armingCheckFailure((NavModes)reporter.failsafeFlags().mode_req_prevent_arming, health_component_t::system,
                                    events::ID("check_modes_cannot_takeoff"),
                                    events::Log::Info, "Mode not suitable for arming");
    }
}
