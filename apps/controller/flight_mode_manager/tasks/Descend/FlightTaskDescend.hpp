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
 * @file FlightTaskDescend.hpp
 */

#pragma once

#include "FlightTask.hpp"
#include "Sticks.hpp"
#include "StickTiltXY.hpp"
#include "StickYaw.hpp"

class FlightTaskDescend : public FlightTask {
public:
    FlightTaskDescend()          = default;
    virtual ~FlightTaskDescend() = default;

    bool update() override;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;

private:
    Sticks      _sticks{this};
    StickTiltXY _stick_tilt_xy{this};
    StickYaw    _stick_yaw{this};

    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTask,
                                    (ParamInt<params_id::MPC_LAND_RC_HELP>)_param_mpc_land_rc_help,
                                    (ParamFloat<params_id::MPC_LAND_SPEED>)_param_mpc_land_speed, ///< velocity for controlled descend
                                    (ParamFloat<params_id::MPC_THR_HOVER>)_param_mpc_thr_hover    ///< thrust at hover equilibrium
    )
};
