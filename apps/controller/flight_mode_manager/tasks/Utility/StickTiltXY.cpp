/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "StickTiltXY.hpp"
#include <geo/geo.h>
#include "Sticks.hpp"

using namespace matrix;

StickTiltXY::StickTiltXY(ModuleParams *parent) :
    ModuleParams(parent) {
}

Vector2f StickTiltXY::generateAccelerationSetpoints(Vector2f stick_xy, const float dt, const float yaw,
                                                    const float yaw_setpoint) {
    Sticks::limitStickUnitLengthXY(stick_xy);
    _man_input_filter.setParameters(dt, _param_mc_man_tilt_tau.get());
    stick_xy = _man_input_filter.update(stick_xy);
    Sticks::rotateIntoHeadingFrameXY(stick_xy, yaw, yaw_setpoint);
    return stick_xy * tanf(math::radians(_param_mpc_man_tilt_max.get())) * CONSTANTS_ONE_G;
}
