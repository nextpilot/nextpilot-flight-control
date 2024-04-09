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
 * @file FlightTaskFailsafe.hpp
 *
 */

#pragma once

#include "FlightTask.hpp"

class FlightTaskFailsafe : public FlightTask {
public:
    FlightTaskFailsafe() = default;

    virtual ~FlightTaskFailsafe() = default;
    bool update() override;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;

private:
    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTask,
                                    (ParamFloat<px4::params::MPC_LAND_SPEED>)_param_mpc_land_speed)
};
