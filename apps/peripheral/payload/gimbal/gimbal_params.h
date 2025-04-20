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

#include <stdint.h>
#include <param/param.h>

namespace gimbal {

struct Parameters {
    int32_t mnt_mode_in;
    int32_t mnt_mode_out;
    int32_t mnt_mav_sysid_v1;
    int32_t mnt_mav_compid_v1;
    int32_t mnt_man_pitch;
    int32_t mnt_man_roll;
    int32_t mnt_man_yaw;
    int32_t mnt_do_stab;
    float   mnt_range_pitch;
    float   mnt_range_roll;
    float   mnt_range_yaw;
    float   mnt_off_pitch;
    float   mnt_off_roll;
    float   mnt_off_yaw;
    int32_t mav_sysid;
    int32_t mav_compid;
    float   mnt_rate_pitch;
    float   mnt_rate_yaw;
    int32_t mnt_rc_in_mode;
    float   mnt_lnd_p_min;
    float   mnt_lnd_p_max;
};

struct ParameterHandles {
    param_t mnt_mode_in;
    param_t mnt_mode_out;
    param_t mnt_mav_sysid_v1;
    param_t mnt_mav_compid_v1;
    param_t mnt_man_pitch;
    param_t mnt_man_roll;
    param_t mnt_man_yaw;
    param_t mnt_do_stab;
    param_t mnt_range_pitch;
    param_t mnt_range_roll;
    param_t mnt_range_yaw;
    param_t mnt_off_pitch;
    param_t mnt_off_roll;
    param_t mnt_off_yaw;
    param_t mav_sysid;
    param_t mav_compid;
    param_t mnt_rate_pitch;
    param_t mnt_rate_yaw;
    param_t mnt_rc_in_mode;
    param_t mnt_lnd_p_min;
    param_t mnt_lnd_p_max;
};

} /* namespace gimbal */
