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
 * @file AttitudeControl.cpp
 */

#include "AttitudeControl.hpp"
#include <mathlib/math/Functions.hpp>

using namespace matrix;

void AttitudeControl::setProportionalGain(const matrix::Vector3f &proportional_gain, const float yaw_weight) {
    _proportional_gain = proportional_gain;
    _yaw_w             = math::constrain(yaw_weight, 0.f, 1.f);

    // compensate for the effect of the yaw weight rescaling the output
    if (_yaw_w > 1e-4f) {
        _proportional_gain(2) /= _yaw_w;
    }
}

matrix::Vector3f AttitudeControl::update(const Quatf &q) const {
    Quatf qd = _attitude_setpoint_q;

    // calculate reduced desired attitude neglecting vehicle's yaw to prioritize roll and pitch
    const Vector3f e_z   = q.dcm_z();
    const Vector3f e_z_d = qd.dcm_z();
    Quatf          qd_red(e_z, e_z_d);

    if (fabsf(qd_red(1)) > (1.f - 1e-5f) || fabsf(qd_red(2)) > (1.f - 1e-5f)) {
        // In the infinitesimal corner case where the vehicle and thrust have the completely opposite direction,
        // full attitude control anyways generates no yaw input and directly takes the combination of
        // roll and pitch leading to the correct desired yaw. Ignoring this case would still be totally safe and stable.
        qd_red = qd;

    } else {
        // transform rotation from current to desired thrust vector into a world frame reduced desired attitude
        qd_red *= q;
    }

    // mix full and reduced desired attitude
    Quatf q_mix = qd_red.inversed() * qd;
    q_mix.canonicalize();
    // catch numerical problems with the domain of acosf and asinf
    q_mix(0) = math::constrain(q_mix(0), -1.f, 1.f);
    q_mix(3) = math::constrain(q_mix(3), -1.f, 1.f);
    qd       = qd_red * Quatf(cosf(_yaw_w * acosf(q_mix(0))), 0, 0, sinf(_yaw_w * asinf(q_mix(3))));

    // quaternion attitude control law, qe is rotation from q to qd
    const Quatf qe = q.inversed() * qd;

    // using sin(alpha/2) scaled rotation axis as attitude error (see quaternion definition by axis angle)
    // also taking care of the antipodal unit quaternion ambiguity
    const Vector3f eq = 2.f * qe.canonical().imag();

    // calculate angular rates setpoint
    Vector3f rate_setpoint = eq.emult(_proportional_gain);

    // Feed forward the yaw setpoint rate.
    // yawspeed_setpoint is the feed forward commanded rotation around the world z-axis,
    // but we need to apply it in the body frame (because _rates_sp is expressed in the body frame).
    // Therefore we infer the world z-axis (expressed in the body frame) by taking the last column of R.transposed (== q.inversed)
    // and multiply it by the yaw setpoint rate (yawspeed_setpoint).
    // This yields a vector representing the commanded rotatation around the world z-axis expressed in the body frame
    // such that it can be added to the rates setpoint.
    if (std::isfinite(_yawspeed_setpoint)) {
        rate_setpoint += q.inversed().dcm_z() * _yawspeed_setpoint;
    }

    // limit rates
    for (int i = 0; i < 3; i++) {
        rate_setpoint(i) = math::constrain(rate_setpoint(i), -_rate_limit(i), _rate_limit(i));
    }

    return rate_setpoint;
}
