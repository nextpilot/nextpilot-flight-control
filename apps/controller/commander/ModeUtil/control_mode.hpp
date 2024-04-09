/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <uORB/topics/offboard_control_mode.h>
#include <uORB/topics/vehicle_control_mode.h>

#include <stdint.h>

namespace mode_util {

void getVehicleControlMode(bool armed, uint8_t nav_state, uint8_t vehicle_type,
                           const offboard_control_mode_s &offboard_control_mode,
                           vehicle_control_mode_s        &vehicle_control_mode);

} // namespace mode_util
