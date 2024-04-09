/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "powerCheck.hpp"
#include <lib/circuit_breaker/circuit_breaker.h>

using namespace time_literals;

void PowerChecks::checkAndReport(const Context &context, Report &reporter) {
    if (circuit_breaker_enabled_by_val(_param_cbrk_supply_chk.get(), CBRK_SUPPLY_CHK_KEY)) {
        return;
    }

    if (context.status().hil_state == vehicle_status_s::HIL_STATE_ON) {
        // Ignore power check in HITL.
        return;
    }

    if (!context.status().power_input_valid) {
        /* EVENT
         * @description
         * Note that USB must be disconnected as well.
         *
         * <profile name="dev">
         * This check can be configured via <param>CBRK_SUPPLY_CHK</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::system, events::ID("check_avionics_power_missing"),
                               events::Log::Error, "Power module not connected");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Power module not connected");
        }

        return;
    }

    system_power_s system_power;

    if (_system_power_sub.copy(&system_power)) {
        // Check avionics rail voltages (if USB isn't connected)
        if (!system_power.usb_connected) {
            float avionics_power_rail_voltage = system_power.voltage5v_v;

            const float low_error_threshold    = 4.5f;
            const float low_warning_threshold  = 4.8f;
            const float high_warning_threshold = 5.4f;

            if (avionics_power_rail_voltage < low_warning_threshold) {
                NavModes affected_groups = NavModes::None;

                if (avionics_power_rail_voltage < low_error_threshold) {
                    affected_groups = NavModes::All;
                }

                /* EVENT
                 * @description
                 * Check the voltage supply to the FMU, it must be above {2:.2} Volt.
                 *
                 * <profile name="dev">
                 * This check can be configured via <param>CBRK_SUPPLY_CHK</param> parameter.
                 * </profile>
                 */
                reporter.healthFailure<float, float>(affected_groups, health_component_t::system,
                                                     events::ID("check_avionics_power_low"),
                                                     events::Log::Error, "Avionics Power low: {1:.2} Volt", avionics_power_rail_voltage, low_warning_threshold);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Avionics Power low: %6.2f Volt",
                                         (double)avionics_power_rail_voltage);
                }

            } else if (avionics_power_rail_voltage > high_warning_threshold) {
                /* EVENT
                 * @description
                 * Check the voltage supply to the FMU, it must be below {2:.2} Volt.
                 *
                 * <profile name="dev">
                 * This check can be configured via <param>CBRK_SUPPLY_CHK</param> parameter.
                 * </profile>
                 */
                reporter.healthFailure<float, float>(NavModes::All, health_component_t::system,
                                                     events::ID("check_avionics_power_high"),
                                                     events::Log::Error, "Avionics Power high: {1:.2} Volt", avionics_power_rail_voltage, high_warning_threshold);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Avionics Power high: %6.2f Volt",
                                         (double)avionics_power_rail_voltage);
                }
            }

            const int power_module_count = math::countSetBits(system_power.brick_valid);

            if (power_module_count < _param_com_power_count.get()) {
                /* EVENT
                 * @description
                 * Available power modules: {1}.
                 * Required power modules: {2}.
                 *
                 * <profile name="dev">
                 * This check can be configured via <param>COM_POWER_COUNT</param> parameter.
                 * </profile>
                 */
                reporter.armingCheckFailure<uint8_t, uint8_t>(NavModes::All, health_component_t::system,
                                                              events::ID("check_avionics_power_redundancy"),
                                                              events::Log::Error, "Power redundancy not met", power_module_count, _param_com_power_count.get());

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Power redundancy not met: %d instead of %" PRId32 "",
                                         power_module_count, _param_com_power_count.get());
                }
            }
        }

    } else {
        /* EVENT
         * @description
         * <profile name="dev">
         * Ensure the ADC is working and the system_power topic is published.
         *
         * This check can be configured via <param>CBRK_SUPPLY_CHK</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::system, events::ID("check_missing_system_power"),
                               events::Log::Error, "System power unavailable");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: system power unavailable");
        }
    }
}
