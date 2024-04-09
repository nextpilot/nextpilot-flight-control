/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "flightTimeCheck.hpp"

void FlightTimeChecks::checkAndReport(const Context &context, Report &reporter) {
    if (_param_com_flt_time_max.get() > FLT_EPSILON && context.status().takeoff_time != 0 &&
        (hrt_absolute_time() - context.status().takeoff_time) > (1_s * _param_com_flt_time_max.get())) {
        reporter.failsafeFlags().flight_time_limit_exceeded = true;
        /* EVENT
         * @description
         * <profile name="dev">
         * This check can be configured via <param>COM_FLT_TIME_MAX</param> parameter.
         * </profile>
         */
        reporter.armingCheckFailure(NavModes::All, health_component_t::system,
                                    events::ID("check_flight_time_limit"),
                                    events::Log::Error, "Maximum flight time reached");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Maximum flight time reached\t");
        }

    } else {
        reporter.failsafeFlags().flight_time_limit_exceeded = false;
    }

    // report warning when approaching max flight time
    if (!reporter.failsafeFlags().flight_time_limit_exceeded && context.status().takeoff_time != 0 && _param_com_flt_time_max.get() > 0) {
        const float remaining_flight_time_sec = (context.status().takeoff_time < hrt_absolute_time()) ?
                                                    _param_com_flt_time_max.get() - 1.e-6f * (hrt_absolute_time() - context.status().takeoff_time) :
                                                    _param_com_flt_time_max.get();

        if (remaining_flight_time_sec < 0.1f * _param_com_flt_time_max.get()) {
            // send warnings every minute until RTL

            const int floored_remaining_flight_time_sec = int(remaining_flight_time_sec);

            if (floored_remaining_flight_time_sec <= 60 && _last_flight_time_warning_sec == -1) {
                // less than or equal to a minute remaining on first pass

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_warning(reporter.mavlink_log_pub(), "Approaching max flight time (system will RTL in %i seconds)\t",
                                        floored_remaining_flight_time_sec);
                }

                /* EVENT
                 * @description
                 * Maximal flight time warning (less than 1min remaining)
                 */
                // events::send<int16_t>(events::ID("commander_max_flight_time_warning_seconds"), events::Log::Warning,
                                      "Approaching max flight time (system will RTL in {1} seconds)", floored_remaining_flight_time_sec);

            } else if ((floored_remaining_flight_time_sec % 60) == 0 && floored_remaining_flight_time_sec >= 60 && floored_remaining_flight_time_sec != _last_flight_time_warning_sec) {
                const int floored_remaining_flight_time_min = (int)(remaining_flight_time_sec * 0.016666667f);

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_warning(reporter.mavlink_log_pub(), "Approaching max flight time (system will RTL in %i minutes)\t",
                                        floored_remaining_flight_time_min);
                }

                /* EVENT
                 * @description
                 * Maximal flight time warning (more than 1min remaining)
                 */
                // events::send<int16_t>(events::ID("commander_max_flight_time_warning_minutes"), events::Log::Warning,
                                      "Approaching max flight time (system will RTL in {1} minutes)", floored_remaining_flight_time_min);
            }

            _last_flight_time_warning_sec = floored_remaining_flight_time_sec;

        } else {
            _last_flight_time_warning_sec = -1; // reset if not in last 10%
        }

    } else {
        _last_flight_time_warning_sec = -1; // reset if not enabled, landed or currently already exceeded
    }
}
