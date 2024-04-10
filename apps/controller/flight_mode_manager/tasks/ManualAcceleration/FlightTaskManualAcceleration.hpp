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
 * @file FlightTaskManualPosition.hpp
 *
 * Flight task for manual position controlled mode.
 *
 */

#pragma once

#include "FlightTaskManualAltitudeSmoothVel.hpp"
#include "StickAccelerationXY.hpp"
#include "StickYaw.hpp"
#include <weather_vane/WeatherVane.hpp>

class FlightTaskManualAcceleration : public FlightTaskManualAltitudeSmoothVel {
public:
    FlightTaskManualAcceleration()          = default;
    virtual ~FlightTaskManualAcceleration() = default;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    bool update() override;

private:
    void _ekfResetHandlerPositionXY(const matrix::Vector2f &delta_xy) override;
    void _ekfResetHandlerVelocityXY(const matrix::Vector2f &delta_vxy) override;

    StickAccelerationXY _stick_acceleration_xy{this};
    StickYaw            _stick_yaw{this};

    WeatherVane _weathervane{this}; /**< weathervane library, used to implement a yaw control law that turns the vehicle nose into the wind */
};
