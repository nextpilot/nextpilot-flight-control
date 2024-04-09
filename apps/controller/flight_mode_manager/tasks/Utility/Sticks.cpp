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
 * @file Sticks.cpp
 */

#include "Sticks.hpp"

using namespace time_literals;
using namespace matrix;

Sticks::Sticks(ModuleParams *parent) :
    ModuleParams(parent) {
}

bool Sticks::checkAndUpdateStickInputs() {
    // Sticks are rescaled linearly and exponentially to [-1,1]
    manual_control_setpoint_s manual_control_setpoint;

    if (_manual_control_setpoint_sub.update(&manual_control_setpoint)) {
        // Linear scale
        _positions(0) = manual_control_setpoint.pitch;
        _positions(1) = manual_control_setpoint.roll;
        _positions(2) = -manual_control_setpoint.throttle;
        _positions(3) = manual_control_setpoint.yaw;

        // Exponential scale
        _positions_expo(0) = math::expo_deadzone(_positions(0), _param_mpc_xy_man_expo.get(), _param_mpc_hold_dz.get());
        _positions_expo(1) = math::expo_deadzone(_positions(1), _param_mpc_xy_man_expo.get(), _param_mpc_hold_dz.get());
        _positions_expo(2) = math::expo_deadzone(_positions(2), _param_mpc_z_man_expo.get(), _param_mpc_hold_dz.get());
        _positions_expo(3) = math::expo_deadzone(_positions(3), _param_mpc_yaw_expo.get(), _param_mpc_hold_dz.get());

        // valid stick inputs are required
        _input_available = manual_control_setpoint.valid && _positions.isAllFinite();

    } else {
        failsafe_flags_s failsafe_flags;

        if (_failsafe_flags_sub.update(&failsafe_flags)) {
            if (failsafe_flags.manual_control_signal_lost) {
                _input_available = false;
            }
        }
    }

    if (!_input_available) {
        // Timeout: set all sticks to zero
        _positions.zero();
        _positions_expo.zero();
    }

    return _input_available;
}

void Sticks::limitStickUnitLengthXY(Vector2f &v) {
    const float vl = v.length();

    if (vl > 1.0f) {
        v /= vl;
    }
}

void Sticks::rotateIntoHeadingFrameXY(Vector2f &v, const float yaw, const float yaw_setpoint) {
    const float yaw_rotate = PX4_ISFINITE(yaw_setpoint) ? yaw_setpoint : yaw;
    v                      = Dcm2f(yaw_rotate) * v;
}
