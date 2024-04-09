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
 * @file RunwayTakeoff.cpp
 * Runway takeoff handling for fixed-wing UAVs with steerable wheels.
 *
 * @author Roman Bapst <roman@px4.io>
 * @author Andreas Antener <andreas@uaventure.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "RunwayTakeoff.h"
#include <mathlib/mathlib.h>
#include <px4_platform_common/events.h>

using namespace time_literals;

namespace runwaytakeoff {

void RunwayTakeoff::init(const hrt_abstime &time_now, const float initial_yaw, const matrix::Vector2d &start_pos_global) {
    initial_yaw_      = initial_yaw;
    start_pos_global_ = start_pos_global;
    takeoff_state_    = RunwayTakeoffState::THROTTLE_RAMP;
    initialized_      = true;
    time_initialized_ = time_now;
    takeoff_time_     = 0;
}

void RunwayTakeoff::update(const hrt_abstime &time_now, const float takeoff_airspeed, const float calibrated_airspeed,
                           const float vehicle_altitude, const float clearance_altitude) {
    switch (takeoff_state_) {
    case RunwayTakeoffState::THROTTLE_RAMP:
        if ((time_now - time_initialized_) > (param_rwto_ramp_time_.get() * 1_s)) {
            takeoff_state_ = RunwayTakeoffState::CLAMPED_TO_RUNWAY;
        }

        break;

    case RunwayTakeoffState::CLAMPED_TO_RUNWAY: {
        const float rotation_airspeed = (param_rwto_rot_airspd_.get() > FLT_EPSILON) ? math::min(param_rwto_rot_airspd_.get(),
                                                                                                 takeoff_airspeed) :
                                                                                       0.9f * takeoff_airspeed;

        if (calibrated_airspeed > rotation_airspeed) {
            takeoff_time_  = time_now;
            takeoff_state_ = RunwayTakeoffState::CLIMBOUT;
            events::send(events::ID("runway_takeoff_reached_airspeed"), events::Log::Info,
                         "Takeoff airspeed reached, climbout");
        }

        break;
    }

    case RunwayTakeoffState::CLIMBOUT:
        if (vehicle_altitude > clearance_altitude) {
            takeoff_state_ = RunwayTakeoffState::FLY;
            events::send(events::ID("runway_takeoff_reached_clearance_altitude"), events::Log::Info, "Reached clearance altitude");
        }

        break;

    default:
        break;
    }
}

bool RunwayTakeoff::controlYaw() {
    // keep controlling yaw directly until we start navigation
    return takeoff_state_ < RunwayTakeoffState::CLIMBOUT;
}

float RunwayTakeoff::getPitch(float external_pitch_setpoint) {
    if (takeoff_state_ <= RunwayTakeoffState::CLAMPED_TO_RUNWAY) {
        return math::radians(param_rwto_psp_.get());
    }

    return external_pitch_setpoint;
}

float RunwayTakeoff::getRoll(float external_roll_setpoint) {
    // until we have enough ground clearance, set roll to 0
    if (takeoff_state_ < RunwayTakeoffState::CLIMBOUT) {
        return 0.0f;
    }

    return external_roll_setpoint;
}

float RunwayTakeoff::getYaw(float external_yaw_setpoint) {
    if (param_rwto_hdg_.get() == 0 && takeoff_state_ < RunwayTakeoffState::CLIMBOUT) {
        return initial_yaw_;

    } else {
        return external_yaw_setpoint;
    }
}

float RunwayTakeoff::getThrottle(const float idle_throttle, const float external_throttle_setpoint) const {
    float throttle = idle_throttle;

    switch (takeoff_state_) {
    case RunwayTakeoffState::THROTTLE_RAMP:

        throttle = interpolateValuesOverAbsoluteTime(idle_throttle, param_rwto_max_thr_.get(), time_initialized_,
                                                     param_rwto_ramp_time_.get());

        break;

    case RunwayTakeoffState::CLAMPED_TO_RUNWAY:
        throttle = param_rwto_max_thr_.get();

        break;

    case RunwayTakeoffState::CLIMBOUT:
        // ramp in throttle setpoint over takeoff rotation transition time
        throttle = interpolateValuesOverAbsoluteTime(param_rwto_max_thr_.get(), external_throttle_setpoint, takeoff_time_,
                                                     param_rwto_rot_time_.get());

        break;

    case RunwayTakeoffState::FLY:
        throttle = external_throttle_setpoint;
    }

    return throttle;
}

bool RunwayTakeoff::resetIntegrators() {
    // reset integrators if we're still on runway
    return takeoff_state_ < RunwayTakeoffState::CLIMBOUT;
}

float RunwayTakeoff::getMinPitch(float min_pitch_in_climbout, float min_pitch) const {
    if (takeoff_state_ < RunwayTakeoffState::CLIMBOUT) {
        // constrain to the taxi pitch setpoint
        return math::radians(param_rwto_psp_.get() - 0.01f);

    } else if (takeoff_state_ < RunwayTakeoffState::FLY) {
        // ramp in the climbout pitch constraint over the rotation transition time
        const float taxi_pitch_min = math::radians(param_rwto_psp_.get() - 0.01f);
        return interpolateValuesOverAbsoluteTime(taxi_pitch_min, min_pitch_in_climbout, takeoff_time_,
                                                 param_rwto_rot_time_.get());

    } else {
        return min_pitch;
    }
}

float RunwayTakeoff::getMaxPitch(const float max_pitch) const {
    if (takeoff_state_ < RunwayTakeoffState::CLIMBOUT) {
        // constrain to the taxi pitch setpoint
        return math::radians(param_rwto_psp_.get() + 0.01f);

    } else if (takeoff_state_ < RunwayTakeoffState::FLY) {
        // ramp in the climbout pitch constraint over the rotation transition time
        const float taxi_pitch_max = math::radians(param_rwto_psp_.get() + 0.01f);
        return interpolateValuesOverAbsoluteTime(taxi_pitch_max, max_pitch, takeoff_time_, param_rwto_rot_time_.get());

    } else {
        return max_pitch;
    }
}

void RunwayTakeoff::reset() {
    initialized_   = false;
    takeoff_state_ = RunwayTakeoffState::THROTTLE_RAMP;
    takeoff_time_  = 0;
}

float RunwayTakeoff::interpolateValuesOverAbsoluteTime(const float start_value, const float end_value,
                                                       const hrt_abstime &start_time, const float interpolation_time) const {
    const float seconds_since_start = hrt_elapsed_time(&start_time) * 1.e-6f;
    const float interpolator        = math::constrain(seconds_since_start / interpolation_time, 0.0f, 1.0f);

    return interpolator * end_value + (1.0f - interpolator) * start_value;
}

} // namespace runwaytakeoff
