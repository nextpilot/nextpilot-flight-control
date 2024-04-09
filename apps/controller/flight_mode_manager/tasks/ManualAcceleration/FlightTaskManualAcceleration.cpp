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
 * @file FlightTaskManualAcceleration.cpp
 */

#include "FlightTaskManualAcceleration.hpp"

using namespace matrix;

bool FlightTaskManualAcceleration::activate(const trajectory_setpoint_s &last_setpoint) {
    bool ret = FlightTaskManualAltitudeSmoothVel::activate(last_setpoint);

    _stick_acceleration_xy.resetPosition();

    if (Vector2f(last_setpoint.velocity).isAllFinite()) {
        _stick_acceleration_xy.resetVelocity(Vector2f(last_setpoint.velocity));

    } else {
        _stick_acceleration_xy.resetVelocity(_velocity.xy());
    }

    _stick_acceleration_xy.resetAcceleration(Vector2f(last_setpoint.acceleration));

    return ret;
}

bool FlightTaskManualAcceleration::update() {
    bool ret = FlightTaskManualAltitudeSmoothVel::update();

    _stick_yaw.generateYawSetpoint(_yawspeed_setpoint, _yaw_setpoint, _sticks.getYawExpo(), _yaw, _is_yaw_good_for_control,
                                   _deltatime);

    _stick_acceleration_xy.generateSetpoints(_sticks.getPitchRollExpo(), _yaw, _yaw_setpoint, _position,
                                             _velocity_setpoint_feedback.xy(), _deltatime);
    _stick_acceleration_xy.getSetpoints(_position_setpoint, _velocity_setpoint, _acceleration_setpoint);

    _constraints.want_takeoff = _checkTakeoff();

    // check if an external yaw handler is active and if yes, let it update the yaw setpoints
    _weathervane.update();

    if (_weathervane.isActive()) {
        _yaw_setpoint = NAN;

        // only enable the weathervane to change the yawrate when position lock is active (and thus the pos. sp. are NAN)
        if (Vector2f(_position_setpoint).isAllFinite()) {
            // vehicle is steady
            _yawspeed_setpoint += _weathervane.getWeathervaneYawrate();
        }
    }

    return ret;
}

void FlightTaskManualAcceleration::_ekfResetHandlerPositionXY(const matrix::Vector2f &delta_xy) {
    _stick_acceleration_xy.resetPosition();
}

void FlightTaskManualAcceleration::_ekfResetHandlerVelocityXY(const matrix::Vector2f &delta_vxy) {
    _stick_acceleration_xy.resetVelocity(_velocity.xy());
}
