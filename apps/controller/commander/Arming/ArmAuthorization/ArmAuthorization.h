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

#include <hrtimer.h>
#include <uORB/uORB.h>

enum arm_auth_methods {
    ARM_AUTH_METHOD_ARM_REQ = 0,
    ARM_AUTH_METHOD_TWO_ARM_REQ,
    ARM_AUTH_METHOD_LAST
};

void    arm_auth_init(orb_advert_t *mav_log_pub, uint8_t *system_id);
void    arm_auth_update(hrt_abstime now, bool param_update = true);
uint8_t arm_auth_check();
