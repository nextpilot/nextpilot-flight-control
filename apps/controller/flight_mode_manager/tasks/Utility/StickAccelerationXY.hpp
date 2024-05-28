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
 * @file StickAccelerationXY.hpp
 * @brief Generate horizontal position, velocity and acceleration setpoints from stick input
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include <module/module_params.hpp>
#include <mathlib/math/filter/AlphaFilter.hpp>
#include <matrix/math.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/takeoff_status.h>
#include "SlewRate.hpp"

using namespace nextpilot;
using namespace nextpilot::global_params;

class StickAccelerationXY : public ModuleParams {
public:
    StickAccelerationXY(ModuleParams *parent);
    ~StickAccelerationXY() = default;

    void resetPosition();
    void resetPosition(const matrix::Vector2f &position);
    void resetVelocity(const matrix::Vector2f &velocity);
    void resetAcceleration(const matrix::Vector2f &acceleration);
    void generateSetpoints(matrix::Vector2f stick_xy, const float yaw, const float yaw_sp, const matrix::Vector3f &pos,
                           const matrix::Vector2f &vel_sp_feedback, const float dt);
    void getSetpoints(matrix::Vector3f &pos_sp, matrix::Vector3f &vel_sp, matrix::Vector3f &acc_sp);

    float getMaxAcceleration() {
        return _param_mpc_acc_hor.get();
    };

    float getMaxJerk() {
        return _param_mpc_jerk_max.get();
    };

    void setVelocityConstraint(float vel) {
        _velocity_constraint = fmaxf(vel, FLT_EPSILON);
    };

private:
    void             applyJerkLimit(const float dt);
    matrix::Vector2f calculateDrag(matrix::Vector2f drag_coefficient, const float dt, const matrix::Vector2f &stick_xy,
                                   const matrix::Vector2f &vel_sp);
    void             applyTiltLimit(matrix::Vector2f &acceleration);
    void             lockPosition(const matrix::Vector3f &pos, const matrix::Vector2f &vel_sp_feedback, const float dt);

    uORB::Subscription _takeoff_status_sub{ORB_ID(takeoff_status)};

    SlewRate<float>    _acceleration_slew_rate_x;
    SlewRate<float>    _acceleration_slew_rate_y;
    AlphaFilter<float> _brake_boost_filter;

    matrix::Vector2f _position_setpoint;
    matrix::Vector2f _velocity_setpoint;
    matrix::Vector2f _acceleration_setpoint;
    matrix::Vector2f _acceleration_setpoint_prev;

    float _velocity_constraint{INFINITY};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::MPC_VEL_MANUAL>)_param_mpc_vel_manual,
        (ParamFloat<params_id::MPC_VEL_MAN_SIDE>)_param_mpc_vel_man_side,
        (ParamFloat<params_id::MPC_VEL_MAN_BACK>)_param_mpc_vel_man_back,
        (ParamFloat<params_id::MPC_ACC_HOR>)_param_mpc_acc_hor,
        (ParamFloat<params_id::MPC_JERK_MAX>)_param_mpc_jerk_max)
};
