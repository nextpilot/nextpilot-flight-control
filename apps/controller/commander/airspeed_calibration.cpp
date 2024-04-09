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
 * @file airspeed_calibration.cpp
 * Airspeed sensor calibration routine
 */

#include "airspeed_calibration.h"
#include "calibration_messages.h"
#include "calibration_routines.h"
#include "commander_helper.h"

#include <px4_platform_common/defines.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <drivers/drv_hrt.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/differential_pressure.h>
#include <systemlib/mavlink_log.h>
#include <parameters/param.h>
#include <systemlib/err.h>

using namespace time_literals;

static const char *sensor_name = "airspeed";

static void feedback_calibration_failed(orb_advert_t *mavlink_log_pub) {
    px4_sleep(5);
    calibration_log_critical(mavlink_log_pub, CAL_QGC_FAILED_MSG, sensor_name);
}

int do_airspeed_calibration(orb_advert_t *mavlink_log_pub) {
    const hrt_abstime calibration_started = hrt_absolute_time();

    int            result              = PX4_OK;
    unsigned       calibration_counter = 0;
    const unsigned maxcount            = 500;

    /* give directions */
    calibration_log_info(mavlink_log_pub, CAL_QGC_STARTED_MSG, sensor_name);

    const unsigned calibration_count = (maxcount * 2) / 3;

    float diff_pres_offset = 0.0f;

    calibration_log_critical(mavlink_log_pub, "[cal] Ensure sensor is not measuring wind");
    px4_usleep(500 * 1000);

    uORB::SubscriptionData<differential_pressure_s> diff_pres_sub{ORB_ID(differential_pressure)};

    while (calibration_counter < calibration_count) {
        if (calibrate_cancel_check(mavlink_log_pub, calibration_started)) {
            return PX4_ERROR;
        }

        if (diff_pres_sub.update()) {
            const differential_pressure_s &diff_pres = diff_pres_sub.get();

            diff_pres_offset += diff_pres.differential_pressure_pa;
            calibration_counter++;

            if (calibration_counter % (calibration_count / 20) == 0) {
                calibration_log_info(mavlink_log_pub, CAL_QGC_PROGRESS_MSG, (calibration_counter * 80) / calibration_count);
            }
        }

        if (hrt_elapsed_time(&calibration_started) > 30_s) {
            feedback_calibration_failed(mavlink_log_pub);
            return PX4_ERROR;
        }

        px4_usleep(10000);
    }

    diff_pres_offset = diff_pres_offset / calibration_count;

    if (PX4_ISFINITE(diff_pres_offset)) {
        // Prevent a completely zero param
        // since this is used to detect a missing calibration
        // This value is numerically down in the noise and has
        // no effect on the sensor performance.
        if (fabsf(diff_pres_offset) < 0.00000001f) {
            diff_pres_offset = 0.00000001f;
        }

        if (param_set(param_find("SENS_DPRES_OFF"), &diff_pres_offset)) {
            calibration_log_critical(mavlink_log_pub, CAL_ERROR_SET_PARAMS_MSG);
            return PX4_ERROR;
        }

    } else {
        feedback_calibration_failed(mavlink_log_pub);
        return PX4_ERROR;
    }

    calibration_log_info(mavlink_log_pub, "[cal] Offset of %d Pascal", (int)diff_pres_offset);

    /* wait 500 ms to ensure parameter propagated through the system */
    px4_usleep(500 * 1000);

    calibration_log_critical(mavlink_log_pub, "[cal] Blow into front of pitot without touching");

    float differential_pressure_sum       = 0.f;
    int   differential_pressure_sum_count = 0;

    calibration_counter = 0;

    /* just take a few samples and make sure pitot tubes are not reversed, timeout after ~30 seconds */
    while (calibration_counter < maxcount) {
        if (calibrate_cancel_check(mavlink_log_pub, calibration_started)) {
            return PX4_ERROR;
        }

        if (diff_pres_sub.update()) {
            const differential_pressure_s &diff_pres = diff_pres_sub.get();

            differential_pressure_sum += diff_pres.differential_pressure_pa;
            differential_pressure_sum_count++;

            const float differential_pressure_pa = (differential_pressure_sum / differential_pressure_sum_count) - diff_pres_offset;

            if ((differential_pressure_sum_count > 10) && (fabsf(differential_pressure_pa) > 50.f)) {
                if (differential_pressure_pa > 0) {
                    calibration_log_info(mavlink_log_pub, "[cal] Positive pressure: OK (%d Pa)", (int)differential_pressure_pa);
                    break;

                } else {
                    /* do not allow negative values */
                    calibration_log_critical(mavlink_log_pub, "[cal] Negative pressure difference detected (%d Pa)",
                                             (int)differential_pressure_pa);
                    calibration_log_critical(mavlink_log_pub, "[cal] Swap static and dynamic ports!");

                    /* the user setup is wrong, wipe the calibration to force a proper re-calibration */
                    diff_pres_offset = 0.0f;

                    if (param_set(param_find("SENS_DPRES_OFF"), &(diff_pres_offset))) {
                        calibration_log_critical(mavlink_log_pub, CAL_ERROR_SET_PARAMS_MSG);
                        return PX4_ERROR;
                    }

                    /* save */
                    calibration_log_info(mavlink_log_pub, CAL_QGC_PROGRESS_MSG, 0);
                    param_save_default();

                    feedback_calibration_failed(mavlink_log_pub);
                    return PX4_ERROR;
                }
            }

            if (calibration_counter % 300 == 0) {
                calibration_log_info(mavlink_log_pub, "[cal] Create air pressure! (got %d, wanted: 50 Pa)",
                                     (int)differential_pressure_pa);
                tune_neutral(true);

                // reset average
                differential_pressure_sum       = 0.f;
                differential_pressure_sum_count = 0;
            }

            calibration_counter++;
        }

        if (hrt_elapsed_time(&calibration_started) > 90_s) {
            feedback_calibration_failed(mavlink_log_pub);
            return PX4_ERROR;
        }

        px4_usleep(10000);
    }

    if (calibration_counter == maxcount) {
        feedback_calibration_failed(mavlink_log_pub);
        return PX4_ERROR;
    }

    calibration_log_info(mavlink_log_pub, CAL_QGC_PROGRESS_MSG, 100);

    calibration_log_info(mavlink_log_pub, CAL_QGC_DONE_MSG, sensor_name);
    tune_neutral(true);

    // This give a chance for the log messages to go out of the queue before someone else stomps on then
    px4_usleep(200000);

    return result;
}
