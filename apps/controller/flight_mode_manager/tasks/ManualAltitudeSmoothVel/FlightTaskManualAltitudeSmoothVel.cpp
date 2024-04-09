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
 * @file FlightManualAltitude.cpp
 */

#include "FlightTaskManualAltitudeSmoothVel.hpp"

#include <float.h>

using namespace matrix;

bool FlightTaskManualAltitudeSmoothVel::activate(const trajectory_setpoint_s &last_setpoint) {
    bool ret = FlightTaskManualAltitude::activate(last_setpoint);

    // Check if the previous FlightTask provided setpoints

    // If the position setpoint is unknown, set to the current postion
    float z_sp_last = PX4_ISFINITE(last_setpoint.position[2]) ? last_setpoint.position[2] : _position(2);

    // If the velocity setpoint is unknown, set to the current velocity
    float vz_sp_last = PX4_ISFINITE(last_setpoint.velocity[2]) ? last_setpoint.velocity[2] : _velocity(2);

    // No acceleration estimate available, set to zero if the setpoint is NAN
    float az_sp_last = PX4_ISFINITE(last_setpoint.acceleration[2]) ? last_setpoint.acceleration[2] : 0.f;

    _smoothing.reset(az_sp_last, vz_sp_last, z_sp_last);

    return ret;
}

void FlightTaskManualAltitudeSmoothVel::_ekfResetHandlerPositionZ(float delta_z) {
    _smoothing.setCurrentPosition(_position(2));
}

void FlightTaskManualAltitudeSmoothVel::_ekfResetHandlerVelocityZ(float delta_vz) {
    _smoothing.setCurrentVelocity(_velocity(2));
}

void FlightTaskManualAltitudeSmoothVel::_updateSetpoints() {
    // Set max accel/vel/jerk
    // Has to be done before _updateTrajectories()
    _updateTrajConstraints();

    _smoothing.setVelSpFeedback(_velocity_setpoint_feedback(2));
    _smoothing.setCurrentPositionEstimate(_position(2));

    // Get yaw setpoint, un-smoothed position setpoints
    FlightTaskManualAltitude::_updateSetpoints();

    _smoothing.update(_deltatime, _velocity_setpoint(2));

    // Fill the jerk, acceleration, velocity and position setpoint vectors
    _setOutputState();
}

void FlightTaskManualAltitudeSmoothVel::_updateTrajConstraints() {
    _smoothing.setMaxJerk(_param_mpc_jerk_max.get());

    _smoothing.setMaxAccelUp(_param_mpc_acc_up_max.get());
    _smoothing.setMaxVelUp(_constraints.speed_up);

    _smoothing.setMaxAccelDown(_param_mpc_acc_down_max.get());
    _smoothing.setMaxVelDown(_constraints.speed_down);
}

void FlightTaskManualAltitudeSmoothVel::_setOutputState() {
    _jerk_setpoint(2)         = _smoothing.getCurrentJerk();
    _acceleration_setpoint(2) = _smoothing.getCurrentAcceleration();
    _velocity_setpoint(2)     = _smoothing.getCurrentVelocity();

    if (!_terrain_hold) {
        if (_terrain_hold_previous) {
            // Reset position setpoint to current position when switching from terrain hold to non-terrain hold
            _smoothing.setCurrentPosition(_position(2));
        }

        _position_setpoint(2) = _smoothing.getCurrentPosition();
    }

    _terrain_hold_previous = _terrain_hold;
}
