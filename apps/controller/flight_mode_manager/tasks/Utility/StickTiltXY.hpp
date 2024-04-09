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
 * @file StickTiltXY.hpp
 * @brief Generate only horizontal acceleration setpoint from stick input
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include <lib/mathlib/math/filter/AlphaFilter.hpp>
#include <matrix/math.hpp>
#include <px4_platform_common/module_params.hpp>

class StickTiltXY : public ModuleParams {
public:
    StickTiltXY(ModuleParams *parent);
    ~StickTiltXY() = default;

    /**
     * Produce acceleration setpoint to tilt a multicopter based on stick input
     *
     * Forward pitch stick input pitches the vehicle's pitch e.g. accelerates the vehicle in its nose direction.
     *
     * @param stick_xy the raw pitch and roll stick positions as input
     * @param dt time in seconds since the last execution
     * @param yaw the current yaw estimate for frame rotation
     * @param yaw_setpoint the current heading setpoint used instead of the estimate if absolute yaw is locked
     * @return NED frame horizontal x, y axis acceleration setpoint
     */
    matrix::Vector2f generateAccelerationSetpoints(matrix::Vector2f stick_xy, const float dt, const float yaw,
                                                   const float yaw_setpoint);

private:
    AlphaFilter<matrix::Vector2f> _man_input_filter;

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::MPC_MAN_TILT_MAX>)_param_mpc_man_tilt_max, ///< maximum tilt allowed for manual flight
        (ParamFloat<params_id::MC_MAN_TILT_TAU>)_param_mc_man_tilt_tau    ///< time constant for stick filter
    )
};
