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
 * @file ecl_roll_controller.cpp
 * Implementation of a simple orthogonal roll PID controller.
 *
 * Authors and acknowledgements in header.
 */

#include "ecl_roll_controller.h"
#include <float.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>

float ECL_RollController::control_attitude(const float dt, const ECL_ControlData &ctl_data) {
    /* Do not calculate control signal with bad inputs */
    if (!(PX4_ISFINITE(ctl_data.roll_setpoint) && PX4_ISFINITE(ctl_data.euler_yaw_rate_setpoint) && PX4_ISFINITE(ctl_data.pitch) && PX4_ISFINITE(ctl_data.roll))) {
        return _body_rate_setpoint;
    }

    /* Calculate the error */
    float roll_error = ctl_data.roll_setpoint - ctl_data.roll;

    /*  Apply P controller: rate setpoint from current error and time constant */
    _euler_rate_setpoint = roll_error / _tc;

    /* Transform setpoint to body angular rates (jacobian) */
    const float roll_body_rate_setpoint_raw = _euler_rate_setpoint - sinf(ctl_data.pitch) * ctl_data.euler_yaw_rate_setpoint;
    _body_rate_setpoint                     = math::constrain(roll_body_rate_setpoint_raw, -_max_rate, _max_rate);

    return _body_rate_setpoint;
}
