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
 * @file rc_calibration.cpp
 * Remote Control calibration routine
 */

#include <px4_platform_common/events.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/time.h>
#include <defines.h>

#include "rc_calibration.h"
#include "commander_helper.h"

#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/manual_control_setpoint.h>
#include <mavlink_log.h>
#include <param/param.h>
#include <systemlib/err.h>

int do_trim_calibration(orb_advert_t *mavlink_log_pub) {
    uORB::Subscription manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};
    px4_usleep(400000);
    manual_control_setpoint_s manual_control_setpoint{};
    bool                      changed = manual_control_setpoint_sub.updated();

    if (!changed) {
        mavlink_log_critical(mavlink_log_pub, "no inputs, aborting\t");
        // events::send(events::ID("commander_cal_no_inputs"), {events::Log::Error, events::LogInternal::Info},
                     "No inputs, aborting RC trim calibration");
                     return PX4_ERROR;
    }

    manual_control_setpoint_sub.copy(&manual_control_setpoint);

    /* load trim values which are active */
    float roll_trim_active;
    param_get(param_find("TRIM_ROLL"), &roll_trim_active);
    float pitch_trim_active;
    param_get(param_find("TRIM_PITCH"), &pitch_trim_active);
    float yaw_trim_active;
    param_get(param_find("TRIM_YAW"), &yaw_trim_active);

    /* get manual control scale values */
    float roll_scale;
    param_get(param_find("FW_MAN_R_SC"), &roll_scale);
    float pitch_scale;
    param_get(param_find("FW_MAN_P_SC"), &pitch_scale);
    float yaw_scale;
    param_get(param_find("FW_MAN_Y_SC"), &yaw_scale);

    /* set parameters: the new trim values are the combination of active trim values
       and the values coming from the remote control of the user
    */
    float p   = manual_control_setpoint.roll * roll_scale + roll_trim_active;
    int   p1r = param_set(param_find("TRIM_ROLL"), &p);
    /*
     we explicitly swap sign here because the trim is added to the actuator controls
     which are moving in an inverse sense to manual pitch inputs
    */
    p       = -manual_control_setpoint.pitch * pitch_scale + pitch_trim_active;
    int p2r = param_set(param_find("TRIM_PITCH"), &p);
    p       = manual_control_setpoint.yaw * yaw_scale + yaw_trim_active;
    int p3r = param_set(param_find("TRIM_YAW"), &p);

    if (p1r != 0 || p2r != 0 || p3r != 0) {
        mavlink_log_critical(mavlink_log_pub, "TRIM: PARAM SET FAIL\t");
        // events::send(events::ID("commander_cal_trim_param_set_failed"), events::Log::Critical,
                     "RC trim calibration: failed to set parameters");
                     return PX4_ERROR;
    }

    mavlink_log_info(mavlink_log_pub, "trim cal done\t");
    // events::send(events::ID("commander_cal_trim_done"), events::Log::Info, "RC trim calibration completed");

    return PX4_OK;
}
