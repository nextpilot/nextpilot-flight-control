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
 * @file FlightTaskFailsafe.cpp
 */

#include "FlightTaskFailsafe.hpp"

bool FlightTaskFailsafe::activate(const trajectory_setpoint_s &last_setpoint) {
    bool ret           = FlightTask::activate(last_setpoint);
    _position_setpoint = _position;
    _velocity_setpoint.zero();
    _acceleration_setpoint = matrix::Vector3f(0.f, 0.f, .3f);
    _yaw_setpoint          = _yaw;
    _yawspeed_setpoint     = 0.f;
    return ret;
}

bool FlightTaskFailsafe::update() {
    bool ret = FlightTask::update();

    if (matrix::Vector2f(_position).isAllFinite()) {
        // stay at current position setpoint
        _velocity_setpoint(0) = _velocity_setpoint(1) = 0.f;
        _acceleration_setpoint(0) = _acceleration_setpoint(1) = 0.f;

    } else if (matrix::Vector2f(_velocity).isAllFinite()) {
        // don't move horizontally
        _position_setpoint(0) = _position_setpoint(1) = NAN;
        _acceleration_setpoint(0) = _acceleration_setpoint(1) = NAN;
    }

    if (PX4_ISFINITE(_position(2))) {
        // stay at current altitude setpoint
        _velocity_setpoint(2)     = 0.f;
        _acceleration_setpoint(2) = NAN;

    } else if (PX4_ISFINITE(_velocity(2))) {
        // land with landspeed
        _velocity_setpoint(2)     = _param_mpc_land_speed.get();
        _position_setpoint(2)     = NAN;
        _acceleration_setpoint(2) = NAN;
    }

    return ret;
}
