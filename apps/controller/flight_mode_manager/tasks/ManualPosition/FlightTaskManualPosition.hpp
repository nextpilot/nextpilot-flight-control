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

#include <lib/collision_prevention/CollisionPrevention.hpp>
#include <lib/weather_vane/WeatherVane.hpp>
#include "FlightTaskManualAltitude.hpp"

class FlightTaskManualPosition : public FlightTaskManualAltitude {
public:
    FlightTaskManualPosition()          = default;
    virtual ~FlightTaskManualPosition() = default;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    bool updateInitialize() override;

protected:
    void _updateXYlock(); /**< applies position lock based on stick and velocity */
    void _updateSetpoints() override;
    void _scaleSticks() override;

    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTaskManualAltitude,
                                    (ParamFloat<params_id::MPC_VEL_MANUAL>)_param_mpc_vel_manual,
                                    (ParamFloat<params_id::MPC_VEL_MAN_SIDE>)_param_mpc_vel_man_side,
                                    (ParamFloat<params_id::MPC_VEL_MAN_BACK>)_param_mpc_vel_man_back,
                                    (ParamFloat<params_id::MPC_ACC_HOR_MAX>)_param_mpc_acc_hor_max,
                                    (ParamFloat<params_id::MPC_HOLD_MAX_XY>)_param_mpc_hold_max_xy)
private:
    uint8_t _reset_counter{0}; /**< counter for estimator resets in xy-direction */

    WeatherVane         _weathervane{this};          /**< weathervane library, used to implement a yaw control law that turns the vehicle nose into the wind */
    CollisionPrevention _collision_prevention{this}; /**< collision avoidance setpoint amendment */
};
