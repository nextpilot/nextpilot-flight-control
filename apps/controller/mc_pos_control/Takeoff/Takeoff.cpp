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
 * @file Takeoff.cpp
 */

#include "Takeoff.hpp"
#include <mathlib/mathlib.h>
#include <geo/geo.h>

void TakeoffHandling::generateInitialRampValue(float velocity_p_gain) {
    velocity_p_gain       = math::max(velocity_p_gain, 0.01f);
    _takeoff_ramp_vz_init = -CONSTANTS_ONE_G / velocity_p_gain;
}

void TakeoffHandling::updateTakeoffState(const bool armed, const bool landed, const bool want_takeoff,
                                         const float takeoff_desired_vz, const bool skip_takeoff, const hrt_abstime &now_us) {
    _spoolup_time_hysteresis.set_state_and_update(armed, now_us);

    switch (_takeoff_state) {
    case TakeoffState::disarmed:
        if (armed) {
            _takeoff_state = TakeoffState::spoolup;

        } else {
            break;
        }

    // FALLTHROUGH
    case TakeoffState::spoolup:
        if (_spoolup_time_hysteresis.get_state()) {
            _takeoff_state = TakeoffState::ready_for_takeoff;

        } else {
            break;
        }

    // FALLTHROUGH
    case TakeoffState::ready_for_takeoff:
        if (want_takeoff) {
            _takeoff_state         = TakeoffState::rampup;
            _takeoff_ramp_progress = 0.f;

        } else {
            break;
        }

    // FALLTHROUGH
    case TakeoffState::rampup:
        if (_takeoff_ramp_progress >= 1.f) {
            _takeoff_state = TakeoffState::flight;

        } else {
            break;
        }

    // FALLTHROUGH
    case TakeoffState::flight:
        if (landed) {
            _takeoff_state = TakeoffState::ready_for_takeoff;
        }

        break;

    default:
        break;
    }

    if (armed && skip_takeoff) {
        _takeoff_state = TakeoffState::flight;
    }

    // TODO: need to consider free fall here
    if (!armed) {
        _takeoff_state = TakeoffState::disarmed;
    }
}

float TakeoffHandling::updateRamp(const float dt, const float takeoff_desired_vz) {
    float upwards_velocity_limit = takeoff_desired_vz;

    if (_takeoff_state < TakeoffState::rampup) {
        upwards_velocity_limit = _takeoff_ramp_vz_init;
    }

    if (_takeoff_state == TakeoffState::rampup) {
        if (_takeoff_ramp_time > dt) {
            _takeoff_ramp_progress += dt / _takeoff_ramp_time;

        } else {
            _takeoff_ramp_progress = 1.f;
        }

        if (_takeoff_ramp_progress < 1.f) {
            upwards_velocity_limit = _takeoff_ramp_vz_init + _takeoff_ramp_progress * (takeoff_desired_vz - _takeoff_ramp_vz_init);
        }
    }

    return upwards_velocity_limit;
}
