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

#include <uORB/topics/failsafe_flags.h>

namespace mode_util {

/**
 * Get per-mode requirements
 * @param vehicle_type one of vehicle_status_s::VEHICLE_TYPE_*
 * @param flags output flags, all mode_req_* entries are set
 */
void getModeRequirements(uint8_t vehicle_type, failsafe_flags_s &flags);

} // namespace mode_util
