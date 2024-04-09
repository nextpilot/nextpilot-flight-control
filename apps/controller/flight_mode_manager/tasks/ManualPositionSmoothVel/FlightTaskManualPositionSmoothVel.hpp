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
 * @file FlightTaskManualPositionSmoothVel.hpp
 *
 * Flight task for smooth manual controlled position.
 */

#pragma once

#include "FlightTaskManualPosition.hpp"
#include <motion_planning/ManualVelocitySmoothingXY.hpp>
#include <motion_planning/ManualVelocitySmoothingZ.hpp>

using matrix::Vector2f;
using matrix::Vector3f;

class FlightTaskManualPositionSmoothVel : public FlightTaskManualPosition {
public:
    FlightTaskManualPositionSmoothVel()          = default;
    virtual ~FlightTaskManualPositionSmoothVel() = default;

    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    void reActivate() override;

protected:
    virtual void _updateSetpoints() override;

    /** Reset position or velocity setpoints in case of EKF reset event */
    void _ekfResetHandlerPositionXY(const matrix::Vector2f &delta_xy) override;
    void _ekfResetHandlerVelocityXY(const matrix::Vector2f &delta_vxy) override;
    void _ekfResetHandlerPositionZ(float delta_z) override;
    void _ekfResetHandlerVelocityZ(float delta_vz) override;

    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTaskManualPosition,
                                    (ParamFloat<px4::params::MPC_JERK_MAX>)_param_mpc_jerk_max,
                                    (ParamFloat<px4::params::MPC_ACC_UP_MAX>)_param_mpc_acc_up_max,
                                    (ParamFloat<px4::params::MPC_ACC_DOWN_MAX>)_param_mpc_acc_down_max)

private:
    void _updateTrajConstraints();
    void _updateTrajConstraintsXY();
    void _updateTrajConstraintsZ();

    void _updateTrajVelFeedback();
    void _updateTrajCurrentPositionEstimate();

    void _updateTrajectories(Vector3f vel_target);

    void _setOutputState();
    void _setOutputStateXY();
    void _setOutputStateZ();

    ManualVelocitySmoothingXY _smoothing_xy; ///< Smoothing in x and y directions
    ManualVelocitySmoothingZ  _smoothing_z;  ///< Smoothing in z direction

    bool _terrain_hold_previous{false}; /**< true when vehicle was controlling height above a static ground position in the previous iteration */
};
