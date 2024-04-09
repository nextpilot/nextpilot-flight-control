/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "FlightTaskManualPositionSmoothVel.hpp"

#include <mathlib/mathlib.h>
#include <float.h>

using namespace matrix;

bool FlightTaskManualPositionSmoothVel::activate(const trajectory_setpoint_s &last_setpoint) {
    bool ret = FlightTaskManualPosition::activate(last_setpoint);

    // Check if the previous FlightTask provided setpoints
    Vector3f accel_prev{last_setpoint.acceleration};
    Vector3f vel_prev{last_setpoint.velocity};
    Vector3f pos_prev{last_setpoint.position};

    for (int i = 0; i < 3; i++) {
        // If the position setpoint is unknown, set to the current postion
        if (!PX4_ISFINITE(pos_prev(i))) { pos_prev(i) = _position(i); }

        // If the velocity setpoint is unknown, set to the current velocity
        if (!PX4_ISFINITE(vel_prev(i))) { vel_prev(i) = _velocity(i); }

        // No acceleration estimate available, set to zero if the setpoint is NAN
        if (!PX4_ISFINITE(accel_prev(i))) { accel_prev(i) = 0.f; }
    }

    _smoothing_xy.reset(Vector2f{accel_prev}, Vector2f{vel_prev}, Vector2f{pos_prev});
    _smoothing_z.reset(accel_prev(2), vel_prev(2), pos_prev(2));

    return ret;
}

void FlightTaskManualPositionSmoothVel::reActivate() {
    FlightTaskManualPosition::reActivate();
    // The task is reacivated while the vehicle is on the ground. To detect takeoff in mc_pos_control_main properly
    // using the generated jerk, reset the z derivatives to zero
    _smoothing_xy.reset(Vector2f(), _velocity.xy(), _position.xy());
    _smoothing_z.reset(0.f, 0.f, _position(2));
}

void FlightTaskManualPositionSmoothVel::_ekfResetHandlerPositionXY(const matrix::Vector2f &delta_xy) {
    _smoothing_xy.setCurrentPosition(_position.xy());
}

void FlightTaskManualPositionSmoothVel::_ekfResetHandlerVelocityXY(const matrix::Vector2f &delta_vxy) {
    _smoothing_xy.setCurrentVelocity(_velocity.xy());
}

void FlightTaskManualPositionSmoothVel::_ekfResetHandlerPositionZ(float delta_z) {
    _smoothing_z.setCurrentPosition(_position(2));
}

void FlightTaskManualPositionSmoothVel::_ekfResetHandlerVelocityZ(float delta_vz) {
    _smoothing_z.setCurrentVelocity(_velocity(2));
}

void FlightTaskManualPositionSmoothVel::_updateSetpoints() {
    // Set max accel/vel/jerk
    // Has to be done before _updateTrajectories()
    _updateTrajConstraints();
    _updateTrajVelFeedback();
    _updateTrajCurrentPositionEstimate();

    // Get yaw setpoint, un-smoothed position setpoints
    FlightTaskManualPosition::_updateSetpoints();

    _updateTrajectories(_velocity_setpoint);

    // Fill the jerk, acceleration, velocity and position setpoint vectors
    _setOutputState();
}

void FlightTaskManualPositionSmoothVel::_updateTrajConstraints() {
    _updateTrajConstraintsXY();
    _updateTrajConstraintsZ();
}

void FlightTaskManualPositionSmoothVel::_updateTrajConstraintsXY() {
    _smoothing_xy.setMaxJerk(_param_mpc_jerk_max.get());
    _smoothing_xy.setMaxAccel(_param_mpc_acc_hor_max.get());
    _smoothing_xy.setMaxVel(_param_mpc_vel_manual.get());
}

void FlightTaskManualPositionSmoothVel::_updateTrajConstraintsZ() {
    _smoothing_z.setMaxJerk(_param_mpc_jerk_max.get());

    _smoothing_z.setMaxAccelUp(_param_mpc_acc_up_max.get());
    _smoothing_z.setMaxVelUp(_constraints.speed_up);

    _smoothing_z.setMaxAccelDown(_param_mpc_acc_down_max.get());
    _smoothing_z.setMaxVelDown(_constraints.speed_down);
}

void FlightTaskManualPositionSmoothVel::_updateTrajVelFeedback() {
    _smoothing_xy.setVelSpFeedback(_velocity_setpoint_feedback.xy());
    _smoothing_z.setVelSpFeedback(_velocity_setpoint_feedback(2));
}

void FlightTaskManualPositionSmoothVel::_updateTrajCurrentPositionEstimate() {
    _smoothing_xy.setCurrentPositionEstimate(_position.xy());
    _smoothing_z.setCurrentPositionEstimate(_position(2));
}

void FlightTaskManualPositionSmoothVel::_updateTrajectories(Vector3f vel_target) {
    _smoothing_xy.update(_deltatime, vel_target.xy());
    _smoothing_z.update(_deltatime, vel_target(2));
}

void FlightTaskManualPositionSmoothVel::_setOutputState() {
    _setOutputStateXY();
    _setOutputStateZ();
}

void FlightTaskManualPositionSmoothVel::_setOutputStateXY() {
    _jerk_setpoint.xy()         = _smoothing_xy.getCurrentJerk();
    _acceleration_setpoint.xy() = _smoothing_xy.getCurrentAcceleration();
    _velocity_setpoint.xy()     = _smoothing_xy.getCurrentVelocity();
    _position_setpoint.xy()     = _smoothing_xy.getCurrentPosition();
}

void FlightTaskManualPositionSmoothVel::_setOutputStateZ() {
    _jerk_setpoint(2)         = _smoothing_z.getCurrentJerk();
    _acceleration_setpoint(2) = _smoothing_z.getCurrentAcceleration();
    _velocity_setpoint(2)     = _smoothing_z.getCurrentVelocity();

    if (!_terrain_hold) {
        if (_terrain_hold_previous) {
            // Reset position setpoint to current position when switching from terrain hold to non-terrain hold
            _smoothing_z.setCurrentPosition(_position(2));
        }

        _position_setpoint(2) = _smoothing_z.getCurrentPosition();
    }

    _terrain_hold_previous = _terrain_hold;
}
