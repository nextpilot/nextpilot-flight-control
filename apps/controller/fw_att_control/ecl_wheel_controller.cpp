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
 * @file ecl_wheel_controller.cpp
 * Implementation of a simple PID wheel controller for heading tracking.
 *
 * Authors and acknowledgements in header.
 */

#include "ecl_wheel_controller.h"
#include <float.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>
#include <matrix/math.hpp>

using matrix::wrap_pi;

float ECL_WheelController::control_bodyrate(const float dt, const ECL_ControlData &ctl_data) {
    /* Do not calculate control signal with bad inputs */
    if (!(PX4_ISFINITE(ctl_data.body_z_rate) && PX4_ISFINITE(ctl_data.groundspeed) && PX4_ISFINITE(ctl_data.groundspeed_scaler))) {
        return math::constrain(_last_output, -1.0f, 1.0f);
    }

    /* input conditioning */
    float min_speed = 1.0f;

    /* Calculate body angular rate error */
    const float rate_error = _body_rate_setpoint - ctl_data.body_z_rate; // body angular rate error

    if (_k_i > 0.0f && ctl_data.groundspeed > min_speed) {
        float id = rate_error * dt * ctl_data.groundspeed_scaler;

        /*
         * anti-windup: do not allow integrator to increase if actuator is at limit
         */
        if (_last_output < -1.0f) {
            /* only allow motion to center: increase value */
            id = math::max(id, 0.0f);

        } else if (_last_output > 1.0f) {
            /* only allow motion to center: decrease value */
            id = math::min(id, 0.0f);
        }

        /* add and constrain */
        _integrator = math::constrain(_integrator + id * _k_i, -_integrator_max, _integrator_max);
    }

    /* Apply PI rate controller and store non-limited output */
    _last_output = _body_rate_setpoint * _k_ff * ctl_data.groundspeed_scaler + ctl_data.groundspeed_scaler * ctl_data.groundspeed_scaler * (rate_error * _k_p + _integrator);

    return math::constrain(_last_output, -1.0f, 1.0f);
}

float ECL_WheelController::control_attitude(const float dt, const ECL_ControlData &ctl_data) {
    /* Do not calculate control signal with bad inputs */
    if (!(PX4_ISFINITE(ctl_data.yaw_setpoint) && PX4_ISFINITE(ctl_data.yaw))) {
        return _body_rate_setpoint;
    }

    /* Calculate the error */
    float yaw_error = wrap_pi(ctl_data.yaw_setpoint - ctl_data.yaw);

    /*  Apply P controller: rate setpoint from current error and time constant */
    _euler_rate_setpoint = yaw_error / _tc;
    _body_rate_setpoint  = _euler_rate_setpoint; // assume 0 pitch and roll angle, thus jacobian is simply identity matrix

    /* limit the rate */
    if (_max_rate > 0.01f) {
        if (_body_rate_setpoint > 0.0f) {
            _body_rate_setpoint = (_body_rate_setpoint > _max_rate) ? _max_rate : _body_rate_setpoint;

        } else {
            _body_rate_setpoint = (_body_rate_setpoint < -_max_rate) ? -_max_rate : _body_rate_setpoint;
        }
    }

    return _body_rate_setpoint;
}
