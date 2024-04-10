/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "StickYaw.hpp"
#include <defines.h>

StickYaw::StickYaw(ModuleParams *parent) :
    ModuleParams(parent) {
}

void StickYaw::generateYawSetpoint(float &yawspeed_setpoint, float &yaw_setpoint, const float stick_yaw,
                                   const float yaw, const bool is_yaw_good_for_control, const float deltatime) {
    _yawspeed_filter.setParameters(deltatime, _param_mpc_man_y_tau.get());
    yawspeed_setpoint = _yawspeed_filter.update(stick_yaw * math::radians(_param_mpc_man_y_max.get()));
    yaw_setpoint      = updateYawLock(yaw, yawspeed_setpoint, yaw_setpoint, is_yaw_good_for_control);
}

float StickYaw::updateYawLock(const float yaw, const float yawspeed_setpoint, const float yaw_setpoint,
                              const bool is_yaw_good_for_control) {
    // Yaw-lock depends on desired yawspeed input. If not locked, yaw_sp is set to NAN.
    if ((fabsf(yawspeed_setpoint) > FLT_EPSILON) || !is_yaw_good_for_control) {
        // no fixed heading when rotating around yaw by stick
        return NAN;

    } else {
        // break down and hold the current heading when no more rotation commanded
        if (!PX4_ISFINITE(yaw_setpoint)) {
            return yaw;

        } else {
            return yaw_setpoint;
        }
    }
}
