/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "failureDetectorCheck.hpp"

void FailureDetectorChecks::checkAndReport(const Context &context, Report &reporter) {
    if (context.status().failure_detector_status & vehicle_status_s::FAILURE_ROLL) {
        /* EVENT
         * @description
         * The vehicle exceeded the maximum configured roll angle.
         *
         * <profile name="dev">
         * This check can be configured via <param>FD_FAIL_R</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_failure_detector_roll"),
                                    events::Log::Critical, "Attitude failure detected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Attitude failure (roll)");
        }

    } else if (context.status().failure_detector_status & vehicle_status_s::FAILURE_PITCH) {
        /* EVENT
         * @description
         * The vehicle exceeded the maximum configured pitch angle.
         *
         * <profile name="dev">
         * This check can be configured via <param>FD_FAIL_P</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_failure_detector_pitch"),
                                    events::Log::Critical, "Attitude failure detected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Attitude failure (pitch)");
        }
    }

    if (context.status().failure_detector_status & vehicle_status_s::FAILURE_ALT) {
        /* EVENT
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_failure_detector_alt"),
                                    events::Log::Critical, "Altitude failure detected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Altitude failure");
        }
    }

    if (context.status().failure_detector_status & vehicle_status_s::FAILURE_EXT) {
        /* EVENT
         * @description
         * <profile name="dev">
         * This check can be configured via <param>FD_EXT_ATS_EN</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_failure_detector_ext"),
                                    events::Log::Critical, "Failure triggered by external system");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Failure triggered by external system");
        }
    }

    reporter.failsafeFlags().fd_critical_failure = context.status().failure_detector_status &
                                                   (vehicle_status_s::FAILURE_ROLL | vehicle_status_s::FAILURE_PITCH | vehicle_status_s::FAILURE_ALT |
                                                    vehicle_status_s::FAILURE_EXT);

    reporter.failsafeFlags().fd_esc_arming_failure = context.status().failure_detector_status &
                                                     vehicle_status_s::FAILURE_ARM_ESC;

    if (reporter.failsafeFlags().fd_esc_arming_failure) {
        /* EVENT
         * @description
         * One or more ESCs failed to arm.
         *
         * <profile name="dev">
         * This check can be configured via <param>FD_ESCS_EN</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::motors_escs, events::ID("check_failure_detector_arm_esc"),
                               events::Log::Critical, "ESC failure");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: ESC failure detected");
        }
    }

    reporter.failsafeFlags().fd_imbalanced_prop = context.status().failure_detector_status &
                                                  vehicle_status_s::FAILURE_IMBALANCED_PROP;

    if (reporter.failsafeFlags().fd_imbalanced_prop) {
        /* EVENT
         * @description
         * Check that all propellers are mounted correctly and are not damaged.
         *
         * <profile name="dev">
         * This check can be configured via <param>FD_IMB_PROP_THR</param> and <param>COM_IMB_PROP_ACT</param> parameters.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::system, events::ID("check_failure_detector_imbalanced_prop"),
                               events::Log::Critical, "Imbalanced propeller detected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Imbalanced propeller detected");
        }
    }

    reporter.failsafeFlags().fd_motor_failure = context.status().failure_detector_status & vehicle_status_s::FAILURE_MOTOR;

    if (reporter.failsafeFlags().fd_motor_failure) {
        /* EVENT
         * @description
         * <profile name="dev">
         * This check can be configured via <param>FD_ACT_EN</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::motors_escs, events::ID("check_failure_detector_motor"),
                               events::Log::Critical, "Motor failure detected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Motor failure detected");
        }
    }
}
