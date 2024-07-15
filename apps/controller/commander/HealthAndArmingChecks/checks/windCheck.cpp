/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "wind_check"
#define LOG_LVL LOG_LVL_INFO

#include "windCheck.hpp"

void WindChecks::checkAndReport(const Context &context, Report &reporter) {
    if (_param_com_wind_warn.get() < FLT_EPSILON && _param_com_wind_max.get() < FLT_EPSILON) {
        reporter.failsafeFlags().wind_limit_exceeded = false;
        return;
    }

    wind_s            wind_estimate;
    const hrt_abstime now = hrt_absolute_time();

    if (_wind_sub.copy(&wind_estimate)) {
        const matrix::Vector2f wind(wind_estimate.windspeed_north, wind_estimate.windspeed_east);

        // publish a warning if it's the first since in air or 60s have passed since the last warning
        const bool warning_timeout_passed = _last_wind_warning == 0 || now - _last_wind_warning > 60_s;

        reporter.failsafeFlags().wind_limit_exceeded = _param_com_wind_max.get() > FLT_EPSILON && wind.longerThan(_param_com_wind_max.get());

        if (reporter.failsafeFlags().wind_limit_exceeded) {
            /* EVENT
             * @description
             * <profile name="dev">
             * This check can be configured via <param>COM_WIND_MAX</param> parameter.
             * </profile>
             */
            reporter.armingCheckFailure<float>(NavModes::All, health_component_t::system,
                                               events::ID("check_wind_too_high"),
                                               events::Log::Warning, "Wind speed is above limit ({1:.1m/s})", wind.norm());

        } else if (_param_com_wind_warn.get() > FLT_EPSILON && wind.longerThan(_param_com_wind_warn.get()) && warning_timeout_passed && context.status().nav_state != vehicle_status_s::NAVIGATION_STATE_AUTO_RTL && context.status().nav_state != vehicle_status_s::NAVIGATION_STATE_AUTO_LAND) {
            events::send<float>(events::ID("check_high_wind_warning"),
                                {events::Log::Warning, events::LogInternal::Info},
                                "High wind speed detected ({1:.1m/s}), landing advised", wind.norm());
            _last_wind_warning = now;
        }
    }
}
