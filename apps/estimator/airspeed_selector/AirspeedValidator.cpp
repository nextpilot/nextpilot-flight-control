/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file AirspeedValidator.cpp
 * Estimates airspeed scale error (from indicated to calibrated airspeed), performs
 * checks on airspeed measurement input and reports airspeed valid or invalid.
 */

#include "AirspeedValidator.hpp"

AirspeedValidator::AirspeedValidator() {
    reset_CAS_scale_check(); // this resets all elements of the Vectors to NAN
}

void AirspeedValidator::update_airspeed_validator(const airspeed_validator_update_data &input_data) {
    // get indicated airspeed from input data (raw airspeed)
    _IAS = input_data.airspeed_indicated_raw;

    update_CAS_scale_validated(input_data.lpos_valid, input_data.ground_velocity, input_data.airspeed_true_raw);
    update_CAS_scale_applied();
    update_CAS_TAS(input_data.air_pressure_pa, input_data.air_temperature_celsius);
    update_wind_estimator(input_data.timestamp, input_data.airspeed_true_raw, input_data.lpos_valid,
                          input_data.ground_velocity, input_data.lpos_evh, input_data.lpos_evv, input_data.q_att);
    update_in_fixed_wing_flight(input_data.in_fixed_wing_flight);
    check_airspeed_data_stuck(input_data.timestamp);
    check_load_factor(input_data.accel_z);
    check_airspeed_innovation(input_data.timestamp, input_data.vel_test_ratio, input_data.mag_test_ratio,
                              input_data.ground_velocity, input_data.lpos_valid);
    update_airspeed_valid_status(input_data.timestamp);
}

void AirspeedValidator::reset_airspeed_to_invalid(const uint64_t timestamp) {
    _airspeed_valid     = false;
    _time_checks_failed = timestamp;
}

void AirspeedValidator::update_wind_estimator(const uint64_t time_now_usec, float airspeed_true_raw, bool lpos_valid,
                                              const matrix::Vector3f &vI, float lpos_evh, float lpos_evv, const Quatf &q_att) {
    _wind_estimator.update(time_now_usec);

    if (lpos_valid && _in_fixed_wing_flight) {
        // airspeed fusion (with raw TAS)
        const float hor_vel_variance = lpos_evh * lpos_evh;
        _wind_estimator.fuse_airspeed(time_now_usec, airspeed_true_raw, vI, hor_vel_variance, q_att);

        // sideslip fusion
        _wind_estimator.fuse_beta(time_now_usec, vI, hor_vel_variance, q_att);
    }
}

// this function returns the current states of the wind estimator to be published in the airspeed module
airspeed_wind_s AirspeedValidator::get_wind_estimator_states(uint64_t timestamp) {
    airspeed_wind_s wind_est = {};

    wind_est.timestamp           = timestamp;
    wind_est.windspeed_north     = _wind_estimator.get_wind()(0);
    wind_est.windspeed_east      = _wind_estimator.get_wind()(1);
    wind_est.variance_north      = _wind_estimator.get_wind_var()(0);
    wind_est.variance_east       = _wind_estimator.get_wind_var()(1);
    wind_est.tas_innov           = _wind_estimator.get_tas_innov();
    wind_est.tas_innov_var       = _wind_estimator.get_tas_innov_var();
    wind_est.beta_innov          = _wind_estimator.get_beta_innov();
    wind_est.beta_innov_var      = _wind_estimator.get_beta_innov_var();
    wind_est.tas_scale_raw       = _wind_estimator.get_tas_scale();
    wind_est.tas_scale_raw_var   = _wind_estimator.get_tas_scale_var();
    wind_est.tas_scale_validated = _CAS_scale_validated;
    return wind_est;
}

void AirspeedValidator::update_CAS_scale_validated(bool lpos_valid, const matrix::Vector3f &vI, float airspeed_true_raw) {
    if (!_in_fixed_wing_flight || !lpos_valid) {
        return;
    }

    // reset every 100s as we assume that all the samples for current check are in similar wind conditions
    if (hrt_elapsed_time(&_begin_current_scale_check) > 100_s) {
        reset_CAS_scale_check();
    }

    const float course_over_ground_rad = matrix::wrap_2pi(atan2f(vI(0), vI(1)));
    const int   segment_index          = int(SCALE_CHECK_SAMPLES * course_over_ground_rad / (2.f * M_PI_F));

    _scale_check_groundspeed(segment_index) = vI.norm();
    _scale_check_TAS(segment_index)         = airspeed_true_raw;

    // run check if all segments are filled
    if (_scale_check_groundspeed.isAllFinite()) {
        float ground_speed_sum = 0.f;
        float TAS_sum          = 0.f;

        for (int i = 0; i < SCALE_CHECK_SAMPLES; i++) {
            ground_speed_sum += _scale_check_groundspeed(i);
            TAS_sum          += _scale_check_TAS(i);
        }

        const float TAS_to_grounspeed_error_current = ground_speed_sum - TAS_sum * _CAS_scale_validated;
        const float TAS_to_grounspeed_error_new     = ground_speed_sum - TAS_sum * _wind_estimator.get_tas_scale();

        // check passes if the average airspeed with the scale applied is closer to groundspeed than without
        if (fabsf(TAS_to_grounspeed_error_new) < fabsf(TAS_to_grounspeed_error_current)) {
            // constrain the scale update to max 0.05 at a time
            const float new_scale_constrained = math::constrain(_wind_estimator.get_tas_scale(), _CAS_scale_validated - 0.05f,
                                                                _CAS_scale_validated + 0.05f);

            _CAS_scale_validated = new_scale_constrained;
        }

        reset_CAS_scale_check();
    }
}

void AirspeedValidator::reset_CAS_scale_check() {
    _scale_check_groundspeed.setAll(NAN);
    _scale_check_TAS.setAll(NAN);

    _begin_current_scale_check = hrt_absolute_time();
}

void AirspeedValidator::update_CAS_scale_applied() {
    switch (_tas_scale_apply) {
    default:

    /* fallthrough */
    case 0:

    /* fallthrough */
    case 1:
        _CAS_scale_applied = _tas_scale_init;
        break;

    case 2:
        _CAS_scale_applied = _CAS_scale_validated;
        break;
    }
}

void AirspeedValidator::update_CAS_TAS(float air_pressure_pa, float air_temperature_celsius) {
    _CAS = calc_CAS_from_IAS(_IAS, _CAS_scale_applied);
    _TAS = calc_TAS_from_CAS(_CAS, air_pressure_pa, air_temperature_celsius);
}

void AirspeedValidator::check_airspeed_data_stuck(uint64_t time_now) {
    // Data stuck test: trigger when IAS is not changing for DATA_STUCK_TIMEOUT (2s) when in fixed-wing flight.
    // Only consider fixed-wing flight as some airspeed sensors have a very low resolution around 0m/s and
    // can output the exact same value for several seconds then.

    if (!_data_stuck_check_enabled) {
        _data_stuck_test_failed = false;
        return;
    }

    if (fabsf(_IAS - _IAS_prev) > FLT_EPSILON || _time_last_unequal_data == 0) {
        _time_last_unequal_data = time_now;
        _IAS_prev               = _IAS;
    }

    _data_stuck_test_failed = hrt_elapsed_time(&_time_last_unequal_data) > DATA_STUCK_TIMEOUT && _in_fixed_wing_flight;
}

void AirspeedValidator::check_airspeed_innovation(uint64_t time_now, float estimator_status_vel_test_ratio,
                                                  float estimator_status_mag_test_ratio, const matrix::Vector3f &vI, bool lpos_valid) {
    // Check normalised innovation levels with requirement for continuous data and use of hysteresis
    // to prevent false triggering.

    if (_wind_estimator.get_wind_estimator_reset()) {
        _time_wind_estimator_initialized = time_now;
    }

    // reset states if check is disabled, we are not flying or wind estimator was just initialized/reset
    if (!_innovation_check_enabled || !_in_fixed_wing_flight || (time_now - _time_wind_estimator_initialized) < 5_s || _tas_innov_integ_threshold <= 0.f) {
        _innovations_check_failed = false;
        _time_last_tas_pass       = time_now;
        _apsd_innov_integ_state   = 0.f;

    } else if (!lpos_valid || estimator_status_vel_test_ratio > 1.f || estimator_status_mag_test_ratio > 1.f) {
        // nav velocity data is likely not good
        // don't run the test but don't reset the check if it had previously failed when nav velocity data was still likely good
        _time_last_tas_pass     = time_now;
        _apsd_innov_integ_state = 0.f;

    } else {
        // nav velocity data is likely good so airspeed innovations are able to be used
        const float      dt_s = math::constrain((time_now - _time_last_aspd_innov_check) / 1e6f, 0.01f, 0.2f); // limit to [5,100] Hz
        matrix::Vector2f wind_2d(_wind_estimator.get_wind());
        matrix::Vector3f air_vel   = vI - matrix::Vector3f{wind_2d(0), wind_2d(1), 0.f};
        const float      tas_innov = fabsf(_TAS - air_vel.norm());

        if (tas_innov > _tas_innov_threshold) {
            _apsd_innov_integ_state += dt_s * (tas_innov - _tas_innov_threshold); // integrate exceedance

        } else {
            // reset integrator used to trigger and record pass if integrator check is disabled
            _apsd_innov_integ_state = 0.f;
        }

        if (_tas_innov_integ_threshold > 0.f && _apsd_innov_integ_state < _tas_innov_integ_threshold) {
            _time_last_tas_pass = time_now;
        }

        _innovations_check_failed = (time_now - _time_last_tas_pass) > TAS_INNOV_FAIL_DELAY;
    }

    _time_last_aspd_innov_check = time_now;
}

void AirspeedValidator::check_load_factor(float accel_z) {
    // Check if the airspeed reading is lower than physically possible given the load factor

    if (!_load_factor_check_enabled) {
        _load_factor_ratio        = 0.5f;
        _load_factor_check_failed = false;
        return;
    }

    if (_in_fixed_wing_flight) {
        float max_lift_ratio       = fmaxf(_CAS, 0.7f) / fmaxf(_airspeed_stall, 1.0f);
        max_lift_ratio            *= max_lift_ratio;
        _load_factor_ratio         = 0.95f * _load_factor_ratio + 0.05f * (fabsf(accel_z) / 9.81f) / max_lift_ratio;
        _load_factor_ratio         = math::constrain(_load_factor_ratio, 0.25f, 2.0f);
        _load_factor_check_failed  = (_load_factor_ratio > 1.1f);

    } else {
        _load_factor_ratio = 0.5f; // reset if not in fixed-wing flight (and not in takeoff condition)
    }
}

void AirspeedValidator::update_airspeed_valid_status(const uint64_t timestamp) {
    if (_data_stuck_test_failed || _innovations_check_failed || _load_factor_check_failed) {
        // at least one check (data stuck, innovation or load factor) failed, so record timestamp
        _time_checks_failed = timestamp;

    } else if (!_data_stuck_test_failed && !_innovations_check_failed && !_load_factor_check_failed) {
        // all checks(data stuck, innovation and load factor) must pass to declare airspeed good
        _time_checks_passed = timestamp;
    }

    if (_airspeed_valid) {
        // A simultaneous load factor and innovaton check fail makes it more likely that a large
        // airspeed measurement fault has developed, so a fault should be declared immediately
        const bool both_checks_failed = (_innovations_check_failed && _load_factor_check_failed);

        // Because the innovation and load factor checks are subject to short duration false positives
        // a timeout period is applied.
        const bool single_check_fail_timeout = (timestamp - _time_checks_passed) > _checks_fail_delay * 1_s;

        if (both_checks_failed || single_check_fail_timeout || _data_stuck_test_failed) {
            _airspeed_valid = false;
        }

    } else if (_checks_clear_delay > 0.f && (timestamp - _time_checks_failed) > _checks_clear_delay * 1_s) {
        // re-enabling is only possible if the clear delay is positive
        _airspeed_valid = true;
    }
}
