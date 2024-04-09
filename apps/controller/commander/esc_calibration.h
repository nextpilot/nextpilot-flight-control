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
 * @file esc_calibration.h
 *
 * Definition of esc calibration
 *
 * @author Roman Bapst <roman@px4.io>
 */

#ifndef ESC_CALIBRATION_H_
#define ESC_CALIBRATION_H_

#include <uORB/topics/actuator_armed.h>

int do_esc_calibration(orb_advert_t *mavlink_log_pub);

bool check_battery_disconnected(orb_advert_t *mavlink_log_pub);

#endif
