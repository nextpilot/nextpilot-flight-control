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
 * @file FlightManualAltitudeSmoothVel.hpp
 *
 * Flight task for manual controlled altitude using the velocity smoothing library
 */

#pragma once

#include "FlightTaskManualAltitude.hpp"
#include <motion_planning/ManualVelocitySmoothingZ.hpp>

class FlightTaskManualAltitudeSmoothVel : public FlightTaskManualAltitude {
public:
    FlightTaskManualAltitudeSmoothVel()          = default;
    virtual ~FlightTaskManualAltitudeSmoothVel() = default;

    bool activate(const trajectory_setpoint_s &last_setpoint) override;

protected:
    virtual void _updateSetpoints() override;

    /** Reset position or velocity setpoints in case of EKF reset event */
    void _ekfResetHandlerPositionZ(float delta_z) override;
    void _ekfResetHandlerVelocityZ(float delta_vz) override;

    void _updateTrajConstraints();
    void _setOutputState();

    ManualVelocitySmoothingZ _smoothing; ///< Smoothing in z direction

    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTaskManualAltitude,
                                    (ParamFloat<px4::params::MPC_JERK_MAX>)_param_mpc_jerk_max,
                                    (ParamFloat<px4::params::MPC_ACC_UP_MAX>)_param_mpc_acc_up_max,
                                    (ParamFloat<px4::params::MPC_ACC_DOWN_MAX>)_param_mpc_acc_down_max)

private:
    bool _terrain_hold_previous{false}; /**< true when vehicle was controlling height above a static ground position in the previous iteration */
};
