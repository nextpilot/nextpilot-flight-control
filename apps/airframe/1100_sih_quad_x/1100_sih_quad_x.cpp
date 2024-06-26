/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "airframe.h"

static param_config_s _params[] = {
    PARAM_CONFIG_INT32(SYS_HITL, 2),
    PARAM_CONFIG_INT32(SIH_VEHICLE_TYPE, 0),
    PARAM_CONFIG_INT32(MAV_0_CONFIG, 0),
};
