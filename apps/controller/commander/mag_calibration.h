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
 * @file mag_calibration.h
 * Magnetometer calibration routine
 */

#ifndef MAG_CALIBRATION_H_
#define MAG_CALIBRATION_H_

#include <math.h>
#include <stdint.h>
#include <uORB/uORB.h>

int do_mag_calibration(orb_advert_t *mavlink_log_pub);
int do_mag_calibration_quick(orb_advert_t *mavlink_log_pub, float heading_radians = 0, float latitude = NAN,
                             float longitude = NAN);

#endif /* MAG_CALIBRATION_H_ */
