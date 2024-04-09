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
 * @file launchDetection.cpp
 * Auto launch detection for catapult/hand-launch vehicles
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 */

#include "LaunchDetector.h"

#include <px4_platform_common/log.h>
#include <px4_platform_common/events.h>

namespace launchdetection {

void LaunchDetector::update(const float dt, const float accel_x) {
    switch (state_) {
    case launch_detection_status_s::STATE_WAITING_FOR_LAUNCH:

        info_delay_counter_s_ += dt;

        /* Inform user that launchdetection is running every kInfoDelay seconds */
        if (info_delay_counter_s_ >= kInfoDelay) {
            // events::send(events::ID("launch_detection_running_info"), events::Log::Info, "Launch detection running");
            info_delay_counter_s_ = 0.f; // reset counter
        }

        /* Detect a acceleration that is longer and stronger as the minimum given by the params */
        if (accel_x > param_fw_laun_ac_thld_.get()) {
            acceleration_detected_counter_ += dt;

            if (acceleration_detected_counter_ > param_fw_laun_ac_t_.get()) {
                if (param_fw_laun_mot_del_.get() > 0.f) {
                    state_ = launch_detection_status_s::STATE_LAUNCH_DETECTED_DISABLED_MOTOR;
                    // events::send<float>(events::ID("launch_detection_wait_for_throttle"), {events::Log::Info, events::LogInternal::Info},
                                        "Launch detected: enablecontrol, waiting {1:.1}s until full throttle", (double)param_fw_laun_mot_del_.get());

                } else {
                    /* No motor delay set: go directly to enablemotors state */
                    state_ = launch_detection_status_s::STATE_FLYING;
                    // events::send(events::ID("launch_detection_no_motor_delay"), {events::Log::Info, events::LogInternal::Info},
                                 "Launch detected: enable motors (no motor delay)");
                }
            }

        } else {
            acceleration_detected_counter_ = 0.f;
        }

        break;

    case launch_detection_status_s::STATE_LAUNCH_DETECTED_DISABLED_MOTOR:
        /* Vehicle is currently controlling attitude but at idle throttle. Waiting until delay is
         * over to allow full throttle */
        motor_delay_counter_ += dt;

        if (motor_delay_counter_ > param_fw_laun_mot_del_.get()) {
            // events::send(events::ID("launch_detection_enable_motors"), {events::Log::Info, events::LogInternal::Info},
                         "Launch detected: enable motors");
                         state_ = launch_detection_status_s::STATE_FLYING;
        }

        info_delay_counter_s_ = kInfoDelay; // reset counter

        break;

    default:                                // state flying
        info_delay_counter_s_ = kInfoDelay; // reset counter
        break;
    }
}

uint LaunchDetector::getLaunchDetected() const {
    return state_;
}

void LaunchDetector::reset() {
    info_delay_counter_s_          = 0.f;
    acceleration_detected_counter_ = 0.f;
    motor_delay_counter_           = 0.f;
    state_                         = launch_detection_status_s::STATE_WAITING_FOR_LAUNCH;
}

} // namespace launchdetection
