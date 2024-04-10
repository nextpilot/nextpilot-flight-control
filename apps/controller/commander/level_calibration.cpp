/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "level_calibration.h"
#include "calibration_messages.h"
#include "calibration_routines.h"
#include "commander_helper.h"

#include <defines.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/time.h>

#include <hrtimer.h>
#include <mathlib/mathlib.h>
#include <geo/geo.h>
#include <matrix/math.hpp>
#include <conversion/rotation.h>

#include <param/param.h>
#include <systemlib/err.h>
#include <mavlink_log.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/uORBSubscription.hpp>
#include <uORB/uORBSubscription.hpp>

using namespace time_literals;
using namespace matrix;
using math::radians;

int do_level_calibration(orb_advert_t *mavlink_log_pub) {
    bool success = false;

    calibration_log_info(mavlink_log_pub, CAL_QGC_STARTED_MSG, "level");

    param_t roll_offset_handle  = param_find("SENS_BOARD_X_OFF");
    param_t pitch_offset_handle = param_find("SENS_BOARD_Y_OFF");

    // get old values
    float roll_offset_current  = 0.f;
    float pitch_offset_current = 0.f;
    param_get(roll_offset_handle, &roll_offset_current);
    param_get(pitch_offset_handle, &pitch_offset_current);

    const Dcmf board_rotation_offset{Eulerf{radians(roll_offset_current), radians(pitch_offset_current), 0.f}};

    float    roll_mean   = 0.f;
    float    pitch_mean  = 0.f;
    unsigned counter     = 0;
    bool     had_motion  = true;
    int      num_retries = 0;

    uORB::SubscriptionBlocking<vehicle_attitude_s> att_sub{ORB_ID(vehicle_attitude)};

    while (had_motion && num_retries++ < 50) {
        Vector2f min_angles{100.f, 100.f};
        Vector2f max_angles{-100.f, -100.f};
        roll_mean                                         = 0.0f;
        pitch_mean                                        = 0.0f;
        counter                                           = 0;
        int                          last_progress_report = -100;
        static constexpr hrt_abstime calibration_duration = 500_ms;
        const hrt_abstime            start                = hrt_absolute_time();

        while (hrt_elapsed_time(&start) < calibration_duration) {
            vehicle_attitude_s att{};

            if (!att_sub.updateBlocking(att, 100000)) {
                // attitude estimator is not running
                calibration_log_critical(mavlink_log_pub, "attitude estimator not running - check system boot");
                calibration_log_critical(mavlink_log_pub, CAL_QGC_FAILED_MSG, "level");
                goto out;
            }

            int progress = 100 * hrt_elapsed_time(&start) / calibration_duration;

            if (progress >= last_progress_report + 20) {
                calibration_log_info(mavlink_log_pub, CAL_QGC_PROGRESS_MSG, progress);
                last_progress_report = progress;
            }

            Eulerf att_euler{Quatf{att.q}};

            // keep min + max angles
            for (int i = 0; i < 2; ++i) {
                if (att_euler(i) < min_angles(i)) { min_angles(i) = att_euler(i); }

                if (att_euler(i) > max_angles(i)) { max_angles(i) = att_euler(i); }
            }

            att_euler(2) = 0.f; // ignore yaw

            att_euler = Eulerf{board_rotation_offset * Dcmf{att_euler}}; // subtract existing board rotation
            roll_mean += att_euler.phi();
            pitch_mean += att_euler.theta();
            ++counter;
        }

        // motion detection: check that (max-min angle) is within a threshold.
        // The difference is typically <0.1 deg while at rest
        if (max_angles(0) - min_angles(0) < math::radians(0.5f) &&
            max_angles(1) - min_angles(1) < math::radians(0.5f)) {
            had_motion = false;
        }
    }

    calibration_log_info(mavlink_log_pub, CAL_QGC_PROGRESS_MSG, 100);

    roll_mean /= counter;
    pitch_mean /= counter;

    if (had_motion) {
        calibration_log_critical(mavlink_log_pub, "motion during calibration");

    } else if (fabsf(roll_mean) > 0.8f) {
        calibration_log_critical(mavlink_log_pub, "excess roll angle");

    } else if (fabsf(pitch_mean) > 0.8f) {
        calibration_log_critical(mavlink_log_pub, "excess pitch angle");

    } else {
        float roll_mean_degrees  = math::degrees(roll_mean);
        float pitch_mean_degrees = math::degrees(pitch_mean);

        if (fabsf(roll_offset_current - roll_mean_degrees) > 0.1f) {
            PX4_INFO("Updating SENS_BOARD_X_OFF %.1f -> %.1f degrees", (double)roll_offset_current, (double)roll_mean_degrees);
        }

        if (fabsf(pitch_offset_current - pitch_mean_degrees) > 0.1f) {
            PX4_INFO("Updating SENS_BOARD_Y_OFF %.1f -> %.1f degrees", (double)pitch_offset_current, (double)pitch_mean_degrees);
        }

        param_set_no_notification(roll_offset_handle, &roll_mean_degrees);
        param_set_no_notification(pitch_offset_handle, &pitch_mean_degrees);
        param_notify_changes();
        success = true;
    }

out:

    if (success) {
        calibration_log_info(mavlink_log_pub, CAL_QGC_DONE_MSG, "level");
        px4_usleep(600000); // give this message enough time to propagate
        return 0;

    } else {
        calibration_log_critical(mavlink_log_pub, CAL_QGC_FAILED_MSG, "level");
        px4_usleep(600000); // give this message enough time to propagate
        return 1;
    }
}
