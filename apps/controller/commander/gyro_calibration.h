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
 * @file gyro_calibration.h
 * Gyroscope calibration routine
 */

#ifndef GYRO_CALIBRATION_H_
#define GYRO_CALIBRATION_H_

#include <stdint.h>
#include <uORB/uORB.h>

int do_gyro_calibration(orb_advert_t *mavlink_log_pub);

#endif /* GYRO_CALIBRATION_H_ */
