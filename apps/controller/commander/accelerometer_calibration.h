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
 * @file accelerometer_calibration.h
 *
 * Definition of accelerometer calibration.
 *
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#ifndef ACCELEROMETER_CALIBRATION_H_
#define ACCELEROMETER_CALIBRATION_H_

#include <stdint.h>
#include <uORB/uORB.h>

int do_accel_calibration(orb_advert_t *mavlink_log_pub);
int do_accel_calibration_quick(orb_advert_t *mavlink_log_pub);

#endif /* ACCELEROMETER_CALIBRATION_H_ */
