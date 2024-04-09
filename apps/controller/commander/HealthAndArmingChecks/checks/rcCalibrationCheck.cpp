/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "rcCalibrationCheck.hpp"

/**
 * Maximum deadzone value
 */
#define RC_INPUT_MAX_DEADZONE_US 500

/**
 * Minimum value
 */
#define RC_INPUT_LOWEST_MIN_US 0

/**
 * Maximum value
 */
#define RC_INPUT_HIGHEST_MAX_US 2500

RcCalibrationChecks::RcCalibrationChecks() {
    char nbuf[20];

    for (unsigned i = 0; i < input_rc_s::RC_INPUT_MAX_CHANNELS; i++) {
        sprintf(nbuf, "RC%d_MIN", i + 1);
        _param_handles[i].min = param_find(nbuf);

        sprintf(nbuf, "RC%d_TRIM", i + 1);
        _param_handles[i].trim = param_find(nbuf);

        sprintf(nbuf, "RC%d_MAX", i + 1);
        _param_handles[i].max = param_find(nbuf);

        sprintf(nbuf, "RC%d_DZ", i + 1);
        _param_handles[i].dz = param_find(nbuf);
    }

    updateParams();
}

void RcCalibrationChecks::checkAndReport(const Context &context, Report &reporter) {
    if (context.isArmed()) {
        return;
    }

    if (_param_com_rc_in_mode.get() != 0 && _param_com_rc_in_mode.get() != 2 && _param_com_rc_in_mode.get() != 3) {
        return;
    }

    for (unsigned i = 0; i < input_rc_s::RC_INPUT_MAX_CHANNELS; i++) {
        float param_min  = _param_values[i].min;
        float param_max  = _param_values[i].max;
        float param_trim = _param_values[i].trim;
        float param_dz   = _param_values[i].dz;

        /* assert min..center..max ordering */
        if (param_min < RC_INPUT_LOWEST_MIN_US) {
            /* EVENT
             * @description
             * Recalibrate the RC.
             */
            reporter.armingCheckFailure<uint8_t, uint16_t>(NavModes::All, health_component_t::remote_control,
                                                           events::ID("check_rc_min_too_small"),
                                                           events::Log::Error, "RC calibration for channel {1} invalid: MIN less than {2}", i + 1, RC_INPUT_LOWEST_MIN_US);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: RC ERROR: RC%d_MIN < %u", i + 1,
                                     RC_INPUT_LOWEST_MIN_US);
            }
        }

        if (param_max > RC_INPUT_HIGHEST_MAX_US) {
            /* EVENT
             * @description
             * Recalibrate the RC.
             */
            reporter.armingCheckFailure<uint8_t, uint16_t>(NavModes::All, health_component_t::remote_control,
                                                           events::ID("check_rc_max_too_high"),
                                                           events::Log::Error, "RC calibration for channel {1} invalid: MAX greater than {2}", i + 1, RC_INPUT_HIGHEST_MAX_US);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: RC ERROR: RC%d_MAX > %u", i + 1,
                                     RC_INPUT_HIGHEST_MAX_US);
            }
        }

        if (param_trim < param_min) {
            /* EVENT
             * @description
             * Recalibrate the RC.
             */
            reporter.armingCheckFailure<uint8_t, int16_t, int16_t>(NavModes::All, health_component_t::remote_control,
                                                                   events::ID("check_rc_trim_too_small"),
                                                                   events::Log::Error, "RC calibration for channel {1} invalid: TRIM less than MIN ({2} less than {3})", i + 1,
                                                                   (int16_t)param_trim, (int16_t)param_min);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: RC ERROR: RC%d_TRIM < MIN (%d/%d)", i + 1,
                                     (int)param_trim, (int)param_min);
            }
        }

        if (param_trim > param_max) {
            /* EVENT
             * @description
             * Recalibrate the RC.
             */
            reporter.armingCheckFailure<uint8_t, int16_t, int16_t>(NavModes::All, health_component_t::remote_control,
                                                                   events::ID("check_rc_trim_too_high"),
                                                                   events::Log::Error, "RC calibration for channel {1} invalid: TRIM greater than MAX ({2} greater than {3})", i + 1,
                                                                   (int16_t)param_trim, (int16_t)param_max);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: RC ERROR: RC%d_TRIM > MAX (%d/%d)", i + 1,
                                     (int)param_trim, (int)param_max);
            }
        }

        /* assert deadzone is sane */
        if (param_dz > RC_INPUT_MAX_DEADZONE_US) {
            /* EVENT
             * @description
             * Recalibrate the RC.
             */
            reporter.armingCheckFailure<uint8_t, uint16_t>(NavModes::All, health_component_t::remote_control,
                                                           events::ID("check_rc_dz_too_high"),
                                                           events::Log::Error, "RC calibration for channel {1} invalid: DZ greater than {2}", i + 1, RC_INPUT_MAX_DEADZONE_US);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: RC ERROR: RC%d_DZ > %u", i + 1,
                                     RC_INPUT_MAX_DEADZONE_US);
            }
        }
    }
}

void RcCalibrationChecks::updateParams() {
    HealthAndArmingCheckBase::updateParams();

    for (unsigned i = 0; i < input_rc_s::RC_INPUT_MAX_CHANNELS; i++) {
        /* initialize values to values failing the check */
        _param_values[i].min  = 0.0f;
        _param_values[i].max  = 0.0f;
        _param_values[i].trim = 0.0f;
        _param_values[i].dz   = RC_INPUT_MAX_DEADZONE_US * 2.0f;

        param_get(_param_handles[i].min, &_param_values[i].min);
        param_get(_param_handles[i].trim, &_param_values[i].trim);
        param_get(_param_handles[i].max, &_param_values[i].max);
        param_get(_param_handles[i].dz, &_param_values[i].dz);
    }
}
