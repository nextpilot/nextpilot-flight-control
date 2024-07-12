/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "system_check"
#define LOG_LVL LOG_LVL_INFO

#include "systemCheck.hpp"

#include "../../Arming/ArmAuthorization/ArmAuthorization.h"
#include <circuit_breaker/circuit_breaker.h>
#include <uORB/topics/vehicle_command_ack.h>

void SystemChecks::checkAndReport(const Context &context, Report &reporter) {
    actuator_armed_s actuator_armed;

    if (_actuator_armed_sub.copy(&actuator_armed)) {
        if (actuator_armed.force_failsafe || actuator_armed.manual_lockdown) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_flight_term_active"),
                                        events::Log::Critical, "Flight termination active");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Flight termination active");
            }
        }
    }

    // USB not connected
    if (!circuit_breaker_enabled_by_val(_param_cbrk_usb_chk.get(), CBRK_USB_CHK_KEY) && context.status().usb_connected) {
        /* EVENT
         * @description
         * Flying with USB is not safe. Disconnect it and reboot the FMU.
         *
         * <profile name="dev">
         * This check can be configured via <param>CBRK_USB_CHK</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_usb_connected"),
                                    events::Log::Error, "USB connected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Flying with USB is not safe");
        }
    }

    // Global position required
    if (!_param_com_arm_wo_gps.get() && !context.isArmed()) {
        if (reporter.failsafeFlags().global_position_invalid) {
            /* EVENT
             * @description
             * <profile name="dev">
             * This check can be configured via <param>COM_ARM_WO_GPS</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_no_global_pos"),
                                        events::Log::Error, "Global position required");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Global position required");
            }

        } else if (reporter.failsafeFlags().home_position_invalid) {
            /* EVENT
             * @description
             * <profile name="dev">
             * This check can be configured via <param>COM_ARM_WO_GPS</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_no_home_pos"),
                                        events::Log::Error, "Home position required");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Home position required");
            }
        }
    }

    // safety button
    if (context.status().safety_button_available && !context.status().safety_off && !context.isArmed()) {
        /* EVENT
         * @description
         * <profile name="dev">
         * This check can be configured via <param>CBRK_IO_SAFETY</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_safety_button"),
                                    events::Log::Info, "Press safety button first");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Press safety button first");
        }
    }

    // avoidance system
    if (context.status().avoidance_system_required) {
        if (context.status().avoidance_system_valid) {
            reporter.setIsPresent(health_component_t::avoidance);

        } else {
            /* EVENT
             * @description
             * <profile name="dev">
             * This check can be configured via <param>COM_OBS_AVOID</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_avoidance_not_ready"),
                                        events::Log::Error, "Avoidance system not ready");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Avoidance system not ready");
            }
        }
    }

    // VTOL in transition
    if (context.status().is_vtol && !context.isArmed()) {
        if (context.status().in_transition_mode) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_vtol_in_transition"),
                                        events::Log::Warning, "Vehicle is in transition state");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Vehicle is in transition state");
            }
        }

        if (!circuit_breaker_enabled_by_val(_param_cbrk_vtolarming.get(), CBRK_VTOLARMING_KEY) && context.status().vehicle_type != vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
            /* EVENT
             * @description
             * <profile name="dev">
             * This check can be configured via <param>CBRK_VTOLARMING</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_vtol_in_fw_mode"),
                                        events::Log::Warning, "Vehicle is not in multicopter mode");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Vehicle is not in multicopter mode");
            }
        }
    }

    // Arm Requirements: authorization
    if (_param_com_arm_auth_req.get() != 0 && !context.isArmed()) {
        if (arm_auth_check() != vehicle_command_ack_s::VEHICLE_CMD_RESULT_ACCEPTED) {
            /* EVENT
             */
            reporter.armingCheckFailure(NavModes::All, health_component_t::system, events::ID("check_system_arm_auth_failed"),
                                        events::Log::Error, "Arm authorization denied");
        }
    }
}
