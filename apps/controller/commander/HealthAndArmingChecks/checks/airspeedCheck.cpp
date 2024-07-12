/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "airspd_check"
#define LOG_LVL LOG_LVL_INFO

#include "airspeedCheck.hpp"
#include <circuit_breaker/circuit_breaker.h>

using namespace time_literals;

AirspeedChecks::AirspeedChecks() :
    _param_fw_arsp_mode_handle(param_find("FW_ARSP_MODE")), _param_fw_airspd_max_handle(param_find("FW_AIRSPD_MAX")) {
}

void AirspeedChecks::checkAndReport(const Context &context, Report &reporter) {
    if (circuit_breaker_enabled_by_val(_param_cbrk_airspd_chk.get(), CBRK_AIRSPD_CHK_KEY)
        || (context.status().vehicle_type != vehicle_status_s::VEHICLE_TYPE_FIXED_WING && !context.status().is_vtol)) {
        return;
    }

    int32_t airspeed_mode = 0;
    param_get(_param_fw_arsp_mode_handle, &airspeed_mode);
    const bool optional = (airspeed_mode == 1);

    airspeed_validated_s airspeed_validated;

    if (_airspeed_validated_sub.copy(&airspeed_validated) && hrt_elapsed_time(&airspeed_validated.timestamp) < 1_s) {
        reporter.setIsPresent(health_component_t::differential_pressure);

        // Maximally allow the airspeed reading to be at FW_AIRSPD_MAX when arming. This is to catch very badly calibrated
        // airspeed sensors, but also high wind conditions that prevent a forward flight of the vehicle.
        float arming_max_airspeed_allowed = 20.f;
        param_get(_param_fw_airspd_max_handle, &arming_max_airspeed_allowed);

        /*
         * Check if airspeed is declared valid or not by airspeed selector.
         */
        if (!PX4_ISFINITE(airspeed_validated.calibrated_airspeed_m_s)) {
            /* EVENT
             */
            reporter.healthFailure(NavModes::All, health_component_t::differential_pressure,
                                   events::ID("check_airspeed_invalid"),
                                   events::Log::Error, "Airspeed invalid");

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Airspeed invalid");
            }
        }

        if (!context.isArmed() && _param_com_arm_arsp_en.get() && fabsf(airspeed_validated.calibrated_airspeed_m_s) > arming_max_airspeed_allowed) {
            /* EVENT
             * @description
             * Current airspeed reading too high. Check if wind is below maximum airspeed and redo airspeed
             * calibration if the measured airspeed does not correspond to wind conditions.
             *
             * <profile name="dev">
             * Measured: {1:.1m/s}, limit: {2:.1m/s}.
             *
             * This check can be configured via <param>COM_ARM_ARSP_EN</param> and <param>FW_AIRSPD_MAX</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure<float, float>(NavModes::None, health_component_t::differential_pressure,
                                                      events::ID("check_airspeed_too_high"),
                                                      events::Log::Error, "Airspeed too high", airspeed_validated.calibrated_airspeed_m_s, arming_max_airspeed_allowed);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Airspeed too high - check airspeed calibration");
            }
        }

    } else if (!optional) {
        /* EVENT
         * @description
         * <profile name="dev">
         * Most likely the airspeed selector module is not running.
         * This check can be configured via <param>CBRK_AIRSPD_CHK</param> parameter.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::differential_pressure,
                               events::ID("check_airspeed_no_data"),
                               events::Log::Error, "No airspeed data");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Airspeed selector module down");
        }
    }
}
