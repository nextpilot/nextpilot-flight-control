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
 * @file esc_calibration.cpp
 *
 * Definition of esc calibration
 *
 * @author Roman Bapst <roman@px4.io>
 */

#include "esc_calibration.h"
#include "calibration_messages.h"
#include "calibration_routines.h"

#include <hrtimer.h>
#include <drivers/drv_pwm_output.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/time.h>
#include <mavlink_log.h>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/actuator_test.h>
#include <parameters/param.h>

using namespace time_literals;

bool check_battery_disconnected(orb_advert_t *mavlink_log_pub) {
    uORB::SubscriptionData<battery_status_s> battery_status_sub{ORB_ID(battery_status)};
    battery_status_sub.update();

    const bool recent_battery_measurement = hrt_absolute_time() < (battery_status_sub.get().timestamp + 1_s);

    if (!recent_battery_measurement) {
        // We have to send this message for now because "battery unavailable" gets ignored by QGC
        calibration_log_critical(mavlink_log_pub, CAL_QGC_FAILED_MSG, "Disconnect battery and try again");
        return false;
    }

    // Make sure battery is reported to be disconnected
    if (recent_battery_measurement && !battery_status_sub.get().connected) {
        return true;
    }

    calibration_log_critical(mavlink_log_pub, CAL_QGC_FAILED_MSG, "Disconnect battery and try again");
    return false;
}

static void set_motor_actuators(uORB::Publication<actuator_test_s> &publisher, float value, bool release_control) {
    actuator_test_s actuator_test{};
    actuator_test.timestamp  = hrt_absolute_time();
    actuator_test.value      = value;
    actuator_test.action     = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
    actuator_test.timeout_ms = 0;

    for (int i = 0; i < actuator_test_s::MAX_NUM_MOTORS; ++i) {
        actuator_test.function = actuator_test_s::FUNCTION_MOTOR1 + i;
        publisher.publish(actuator_test);
    }
}

int do_esc_calibration(orb_advert_t *mavlink_log_pub) {
    // 1 Initialization
    bool calibration_failed = false;

    uORB::Publication<actuator_test_s> actuator_test_pub{ORB_ID(actuator_test)};
    // since we publish multiple at once, make sure the output driver subscribes before we publish
    actuator_test_pub.advertise();

    uORB::SubscriptionData<battery_status_s> battery_status_sub{ORB_ID(battery_status)};
    battery_status_sub.update();
    const bool  battery_connected_before_calibration = battery_status_sub.get().connected;
    const float current_before_calibration           = battery_status_sub.get().current_a;

    calibration_log_info(mavlink_log_pub, CAL_QGC_STARTED_MSG, "esc");

    px4_usleep(10_ms);

    // 2 Set motors to high
    set_motor_actuators(actuator_test_pub, 1.f, false);
    calibration_log_info(mavlink_log_pub, "[cal] Connect battery now");

    hrt_abstime timeout_start = hrt_absolute_time();

    // 3 Wait for user to connect power
    while (true) {
        hrt_abstime now = hrt_absolute_time();
        battery_status_sub.update();

        if (now > (timeout_start + 1_s) && (battery_status_sub.get().current_a > current_before_calibration + 1.f)) {
            // Safety termination, current rises immediately, user didn't unplug power before
            calibration_failed = true;
            break;
        }

        if (!battery_connected_before_calibration && battery_status_sub.get().connected) {
            // Battery connection detected we can go to the next step immediately
            break;
        }

        if (now > (timeout_start + 6_s)) {
            // Timeout, we continue since maybe the battery cannot be detected properly
            // If we abort here and the ESCs are infact connected and started calibrating
            // they will measure the disarmed value as the lower limit instead of the fixed 1000us
            break;
        }

        px4_usleep(50_ms);
    }

    // 4 Wait for ESCs to measure high signal
    if (!calibration_failed) {
        calibration_log_info(mavlink_log_pub, "[cal] Battery connected");
        px4_usleep(3_s);
    }

    // 5 Set motors to low
    if (!calibration_failed) {
        set_motor_actuators(actuator_test_pub, 0.f, false);
    }

    // 6 Wait for ESCs to measure low signal
    if (!calibration_failed) {
        px4_usleep(5_s);
    }

    // 7 release control
    set_motor_actuators(actuator_test_pub, 0.f, true);

    // 8 Report
    if (calibration_failed) {
        calibration_log_critical(mavlink_log_pub, CAL_QGC_FAILED_MSG, "Timeout waiting for battery");
        return PX4_ERROR;

    } else {
        calibration_log_info(mavlink_log_pub, CAL_QGC_DONE_MSG, "esc");
        return PX4_OK;
    }
}
