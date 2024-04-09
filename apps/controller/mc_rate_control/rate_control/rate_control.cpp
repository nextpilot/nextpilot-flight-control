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
 * @file RateControl.cpp
 */

#include "rate_control.hpp"

using namespace matrix;

void RateControl::setGains(const Vector3f &P, const Vector3f &I, const Vector3f &D) {
    _gain_p = P;
    _gain_i = I;
    _gain_d = D;
}

void RateControl::setSaturationStatus(const Vector3<bool> &saturation_positive,
                                      const Vector3<bool> &saturation_negative) {
    _control_allocator_saturation_positive = saturation_positive;
    _control_allocator_saturation_negative = saturation_negative;
}

void RateControl::setPositiveSaturationFlag(size_t axis, bool is_saturated) {
    if (axis < 3) {
        _control_allocator_saturation_positive(axis) = is_saturated;
    }
}

void RateControl::setNegativeSaturationFlag(size_t axis, bool is_saturated) {
    if (axis < 3) {
        _control_allocator_saturation_negative(axis) = is_saturated;
    }
}

Vector3f RateControl::update(const Vector3f &rate, const Vector3f &rate_sp, const Vector3f &angular_accel,
                             const float dt, const bool landed) {
    // angular rates error
    Vector3f rate_error = rate_sp - rate;

    // PID control with feed forward
    const Vector3f torque = _gain_p.emult(rate_error) + _rate_int - _gain_d.emult(angular_accel) + _gain_ff.emult(rate_sp);

    // update integral only if we are not landed
    if (!landed) {
        updateIntegral(rate_error, dt);
    }

    return torque;
}

void RateControl::updateIntegral(Vector3f &rate_error, const float dt) {
    for (int i = 0; i < 3; i++) {
        // prevent further positive control saturation
        if (_control_allocator_saturation_positive(i)) {
            rate_error(i) = math::min(rate_error(i), 0.f);
        }

        // prevent further negative control saturation
        if (_control_allocator_saturation_negative(i)) {
            rate_error(i) = math::max(rate_error(i), 0.f);
        }

        // I term factor: reduce the I gain with increasing rate error.
        // This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
        // change (noticeable in a bounce-back effect after a flip).
        // The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
        // with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
        // and up to 200 deg error leads to <25% reduction of I.
        float i_factor = rate_error(i) / math::radians(400.f);
        i_factor       = math::max(0.0f, 1.f - i_factor * i_factor);

        // Perform the integration using a first order method
        float rate_i = _rate_int(i) + i_factor * _gain_i(i) * rate_error(i) * dt;

        // do not propagate the result if out of range or invalid
        if (PX4_ISFINITE(rate_i)) {
            _rate_int(i) = math::constrain(rate_i, -_lim_int(i), _lim_int(i));
        }
    }
}

void RateControl::getRateControlStatus(rate_ctrl_status_s &rate_ctrl_status) {
    rate_ctrl_status.rollspeed_integ  = _rate_int(0);
    rate_ctrl_status.pitchspeed_integ = _rate_int(1);
    rate_ctrl_status.yawspeed_integ   = _rate_int(2);
}
