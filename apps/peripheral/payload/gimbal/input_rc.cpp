/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "input_rc.h"

#include <math.h>
#include <errno.h>
#include <mathlib/mathlib.h>
#include <matrix/matrix/math.hpp>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/log.h>

namespace gimbal {

InputRC::InputRC(Parameters &parameters) :
    InputBase(parameters) {
}

InputRC::~InputRC() {
    if (_manual_control_setpoint_sub >= 0) {
        orb_unsubscribe(_manual_control_setpoint_sub);
    }
}

int InputRC::initialize() {
    _manual_control_setpoint_sub = orb_subscribe(ORB_ID(manual_control_setpoint));

    if (_manual_control_setpoint_sub < 0) {
        return -errno;
    }

    return 0;
}

InputRC::UpdateResult InputRC::update(unsigned int timeout_ms, ControlData &control_data, bool already_active) {
    px4_pollfd_struct_t polls[1];
    polls[0].fd     = _manual_control_setpoint_sub;
    polls[0].events = POLLIN;

    int ret = px4_poll(polls, 1, timeout_ms);

    if (ret < 0) {
        return UpdateResult::NoUpdate;
    }

    if (ret == 0) {
        // If we have been active before, we stay active, unless someone steals
        // the control away.
        if (already_active) {
            return UpdateResult::UpdatedActive;
        }
    }

    if (polls[0].revents & POLLIN) {
        return _read_control_data_from_subscription(control_data, already_active);
    }

    return UpdateResult::NoUpdate;
}

InputRC::UpdateResult InputRC::_read_control_data_from_subscription(ControlData &control_data, bool already_active) {
    manual_control_setpoint_s manual_control_setpoint{};
    orb_copy(ORB_ID(manual_control_setpoint), _manual_control_setpoint_sub, &manual_control_setpoint);
    control_data.type = ControlData::Type::Angle;

    float new_aux_values[3];

    for (int i = 0; i < 3; ++i) {
        new_aux_values[i] = _get_aux_value(manual_control_setpoint, i);
    }

    // If we were already active previously, we just update normally. Otherwise, there needs to be
    // a major stick movement to re-activate manual (or it's running for the very first time).

    // Detect a big stick movement
    const bool major_movement = [&]() {
        for (int i = 0; i < 3; ++i) {
            if (fabsf(_last_set_aux_values[i] - new_aux_values[i]) > 0.25f) {
                return true;
            }
        }

        return false;
    }();

    if (already_active || major_movement) {
        control_data.sysid_primary_control  = _parameters.mav_sysid;
        control_data.compid_primary_control = _parameters.mav_compid;

        if (_parameters.mnt_rc_in_mode == 0) {
            // We scale manual input from roll -180..180, pitch -90..90, yaw, -180..180 degrees.
            matrix::Eulerf euler(new_aux_values[0] * math::radians(180.f),
                                 new_aux_values[1] * math::radians(90.f),
                                 new_aux_values[2] * math::radians(180.f));

            matrix::Quatf q(euler);
            q.copyTo(control_data.type_data.angle.q);

            control_data.type_data.angle.frames[0] = ControlData::TypeData::TypeAngle::Frame::AngleAbsoluteFrame;
            control_data.type_data.angle.frames[1] = ControlData::TypeData::TypeAngle::Frame::AngleAbsoluteFrame;
            control_data.type_data.angle.frames[2] = ControlData::TypeData::TypeAngle::Frame::AngleBodyFrame;

            control_data.type_data.angle.angular_velocity[0] = NAN;
            control_data.type_data.angle.angular_velocity[1] = NAN;
            control_data.type_data.angle.angular_velocity[2] = NAN;

        } else {
            control_data.type_data.angle.q[0] = NAN;
            control_data.type_data.angle.q[1] = NAN;
            control_data.type_data.angle.q[2] = NAN;
            control_data.type_data.angle.q[3] = NAN;

            control_data.type_data.angle.angular_velocity[0] = 0.f;
            control_data.type_data.angle.angular_velocity[1] = math::radians(_parameters.mnt_rate_pitch) * new_aux_values[1];
            control_data.type_data.angle.angular_velocity[2] = math::radians(_parameters.mnt_rate_yaw) * new_aux_values[2];

            control_data.type_data.angle.frames[0] = ControlData::TypeData::TypeAngle::Frame::AngularRate;
            control_data.type_data.angle.frames[1] = ControlData::TypeData::TypeAngle::Frame::AngularRate;
            control_data.type_data.angle.frames[2] = ControlData::TypeData::TypeAngle::Frame::AngularRate;
        }

        for (int i = 0; i < 3; ++i) {
            // We always use follow mode with RC input for now.
            _last_set_aux_values[i] = new_aux_values[i];
        }

        return UpdateResult::UpdatedActive;

    } else {
        return UpdateResult::NoUpdate;
    }
}

float InputRC::_get_aux_value(const manual_control_setpoint_s &manual_control_setpoint, int channel_idx) {
    int32_t aux_channel = [&]() {
        switch (channel_idx) {
        case 0:
            return _parameters.mnt_man_roll;

        case 1:
            return _parameters.mnt_man_pitch;

        case 2:
            return _parameters.mnt_man_yaw;

        default:
            return int32_t(0);
        }
    }();

    switch (aux_channel) {
    case 1:
        return manual_control_setpoint.aux1;

    case 2:
        return manual_control_setpoint.aux2;

    case 3:
        return manual_control_setpoint.aux3;

    case 4:
        return manual_control_setpoint.aux4;

    case 5:
        return manual_control_setpoint.aux5;

    case 6:
        return manual_control_setpoint.aux6;

    default:
        return 0.0f;
    }
}

void InputRC::print_status() const {
    PX4_INFO("Input: RC (channels: roll=%" PRIi32 ", pitch=%" PRIi32 ", yaw=%" PRIi32 ")", _parameters.mnt_man_roll,
             _parameters.mnt_man_pitch, _parameters.mnt_man_yaw);
}

} /* namespace gimbal */
