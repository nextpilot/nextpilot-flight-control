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

static param_config_t _params[] = {
    PARAM_CONFIG_INT32(SYS_HITL, 2),
    PARAM_CONFIG_INT32(SIH_VEHICLE_TYPE, 3),
    PARAM_CONFIG_INT32(MAV_0_CONFIG, 0),

    // Battery
    PARAM_CONFIG_INT32(BAT_N_CELLS, 6),
    PARAM_CONFIG_FLOAT(BAT_V_CHARGED, 4.2),
    PARAM_CONFIG_FLOAT(BAT_V_EMPTY, 3.7),

    // RC
    PARAM_CONFIG_INT32(RC_MAP_ROLL, 1),
    PARAM_CONFIG_INT32(RC_MAP_PITCH, 2),
    PARAM_CONFIG_INT32(RC_MAP_YAW, 3),
    PARAM_CONFIG_INT32(RC_MAP_THROTTLE, 4),
};

AIRFRAME_DEFINE_PARAM(sih_standard_vtol, 1103, _params);
