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
 * @file FlightTaskDescend.cpp
 */

#include "FlightTaskDescend.hpp"

bool FlightTaskDescend::activate(const trajectory_setpoint_s &last_setpoint) {
    bool ret           = FlightTask::activate(last_setpoint);
    _gear.landing_gear = landing_gear_s::GEAR_DOWN;
    return ret;
}

bool FlightTaskDescend::update() {
    bool ret = FlightTask::update();

    if (PX4_ISFINITE(_velocity(2))) {
        // land with landspeed
        _velocity_setpoint(2)     = _param_mpc_land_speed.get();
        _acceleration_setpoint(2) = NAN;

    } else {
        // descend with constant acceleration (crash landing)
        _velocity_setpoint(2)     = NAN;
        _acceleration_setpoint(2) = .15f;
    }

    // Nudging
    if (_param_mpc_land_rc_help.get() && _sticks.checkAndUpdateStickInputs()) {
        _stick_yaw.generateYawSetpoint(_yawspeed_setpoint, _yaw_setpoint, _sticks.getYawExpo(), _yaw,
                                       _is_yaw_good_for_control, _deltatime);
        _acceleration_setpoint.xy() = _stick_tilt_xy.generateAccelerationSetpoints(_sticks.getPitchRoll(), _deltatime, _yaw,
                                                                                   _yaw_setpoint);

        // Stick full up -1 -> stop, stick full down 1 -> double the value
        _velocity_setpoint(2) *= (1 - _sticks.getThrottleZeroCenteredExpo());
        _acceleration_setpoint(2) -= _sticks.getThrottleZeroCentered() * 10.f;

    } else {
        _acceleration_setpoint = matrix::Vector3f(0.f, 0.f, NAN); // stay level to minimize horizontal drift
        _yawspeed_setpoint     = NAN;

        // keep heading
        if (!PX4_ISFINITE(_yaw_setpoint)) {
            _yaw_setpoint = _yaw;
        }
    }

    return ret;
}
