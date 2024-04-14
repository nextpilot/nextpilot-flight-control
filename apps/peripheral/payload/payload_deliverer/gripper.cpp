/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "gripper.h"

void Gripper::init(const GripperConfig &config) {
    switch (config.type) {
    case GripperConfig::GripperType::SERVO:
        break;

    case GripperConfig::GripperType::UNDEFINED:

    // FALL THROUGH
    default:
        _valid = false;
        return;
    }

    switch (config.sensor) {
    case GripperConfig::GripperSensorType::ENCODER:
        // Handle encoder sensor setup
        _has_feedback_sensor = true;
        break;

    case GripperConfig::GripperSensorType::NONE:
    default:
        // No feedback sensor
        _has_feedback_sensor = false;
        break;
    }

    _timeout_us = config.timeout_us;

    // Reset internal states
    _state                = GripperState::IDLE;
    _last_command_time    = 0;
    _released_state_cache = false;
    _grabbed_state_cache  = false;

    // We have valid gripper type & sensor configuration
    _valid = true;
}

void Gripper::deinit() {
    // Reset Config variables
    _has_feedback_sensor = false;
    _timeout_us          = 0;

    // Reset internal states
    _state                = GripperState::IDLE;
    _last_command_time    = 0;
    _released_state_cache = false;
    _grabbed_state_cache  = false;

    // Mark gripper instance as invalid
    _valid = false;
}

void Gripper::grab() {
    if (_state == GripperState::GRABBING || _state == GripperState::GRABBED) {
        return;
    }

    publish_gripper_command(gripper_s::COMMAND_GRAB);
    _state             = GripperState::GRABBING;
    _last_command_time = hrt_absolute_time();
}

void Gripper::release() {
    if (_state == GripperState::RELEASING || _state == GripperState::RELEASED) {
        return;
    }

    publish_gripper_command(gripper_s::COMMAND_RELEASE);
    _state             = GripperState::RELEASING;
    _last_command_time = hrt_absolute_time();
}

void Gripper::update() {
    const bool command_timed_out = (hrt_elapsed_time(&_last_command_time) >= _timeout_us);

    // Handle transition from intermediate state to definite state
    switch (_state) {
    case GripperState::GRABBING:
        if (_has_feedback_sensor) {
            // Handle feedback sensor input, return true for now (not supported)
            _grabbed_state_cache = true;
            _state               = GripperState::GRABBED;
            break;
        }

        if (command_timed_out) {
            _grabbed_state_cache = true;
            _state               = GripperState::GRABBED;
        }

        break;

    case GripperState::RELEASING:
        if (_has_feedback_sensor) {
            // Handle feedback sensor input, return true for now (not supported)
            _released_state_cache = true;
            _state                = GripperState::RELEASED;
            break;
        }

        if (command_timed_out) {
            _released_state_cache = true;
            _state                = GripperState::RELEASED;
        }

        break;

    default:
        // DO NOTHING
        break;
    }
}

void Gripper::publish_gripper_command(const int8_t gripper_command) {
    gripper_s gripper{};
    gripper.timestamp = hrt_absolute_time();
    gripper.command   = gripper_command;
    _gripper_pub.publish(gripper);
}

const char *Gripper::get_state_str() const {
    switch (_state) {
    case GripperState::GRABBING:
        return "GRABBING";

    case GripperState::GRABBED:
        return "GRABBED";

    case GripperState::RELEASING:
        return "RELEASING";

    case GripperState::RELEASED:
        return "RELEASED";

    case GripperState::IDLE:
        return "IDLE";

    default:
        return "UNKNOWN";
    }
}
