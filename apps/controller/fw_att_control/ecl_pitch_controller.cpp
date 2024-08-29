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
 * @file ecl_pitch_controller.cpp
 * Implementation of a simple orthogonal pitch PID controller.
 *
 * Authors and acknowledgements in header.
 */

#include "ecl_pitch_controller.h"
#include <float.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>

float ECL_PitchController::control_attitude(const float dt, const ECL_ControlData &ctl_data) {
    /* Do not calculate control signal with bad inputs */
    if (!(PX4_ISFINITE(ctl_data.pitch_setpoint) && PX4_ISFINITE(ctl_data.roll) && PX4_ISFINITE(ctl_data.pitch) && PX4_ISFINITE(ctl_data.euler_yaw_rate_setpoint))) {
        return _body_rate_setpoint;
    }

    /* Calculate the error */
    float pitch_error = ctl_data.pitch_setpoint - ctl_data.pitch;

    /*  Apply P controller: rate setpoint from current error and time constant */
    _euler_rate_setpoint = pitch_error / _tc;

    /* Transform setpoint to body angular rates (jacobian) */
    const float pitch_body_rate_setpoint_raw = cosf(ctl_data.roll) * _euler_rate_setpoint + cosf(ctl_data.pitch) * sinf(ctl_data.roll) * ctl_data.euler_yaw_rate_setpoint;
    _body_rate_setpoint                      = math::constrain(pitch_body_rate_setpoint_raw, -_max_rate_neg, _max_rate);

    return _body_rate_setpoint;
}
