/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "output_rc.h"

#include <uORB/topics/gimbal_controls.h>
#include <px4_platform_common/defines.h>
#include <matrix/matrix/math.hpp>

using math::constrain;

namespace gimbal {

OutputRC::OutputRC(const Parameters &parameters) :
    OutputBase(parameters) {
}

void OutputRC::update(const ControlData &control_data, bool new_setpoints, uint8_t &gimbal_device_id) {
    if (new_setpoints) {
        _set_angle_setpoints(control_data);
    }

    _handle_position_update(control_data);

    hrt_abstime t = hrt_absolute_time();
    _calculate_angle_output(t);

    _stream_device_attitude_status();

    // If the output is RC, then it means we are also the gimbal device. gimbal_device_id = (uint8_t)_parameters.mnt_mav_compid_v1;

    // _angle_outputs are in radians, gimbal_controls are in [-1, 1]
    gimbal_controls_s gimbal_controls{};
    gimbal_controls.control[gimbal_controls_s::INDEX_ROLL] = constrain(
        (_angle_outputs[0] + math::radians(_parameters.mnt_off_roll)) *
            (1.0f / (math::radians(_parameters.mnt_range_roll / 2.0f))),
        -1.f, 1.f);
    gimbal_controls.control[gimbal_controls_s::INDEX_PITCH] = constrain(
        (_angle_outputs[1] + math::radians(_parameters.mnt_off_pitch)) *
            (1.0f / (math::radians(_parameters.mnt_range_pitch / 2.0f))),
        -1.f, 1.f);
    gimbal_controls.control[gimbal_controls_s::INDEX_YAW] = constrain(
        (_angle_outputs[2] + math::radians(_parameters.mnt_off_yaw)) *
            (1.0f / (math::radians(_parameters.mnt_range_yaw / 2.0f))),
        -1.f, 1.f);
    gimbal_controls.timestamp = hrt_absolute_time();
    _gimbal_controls_pub.publish(gimbal_controls);

    _last_update = t;
}

void OutputRC::print_status() const {
    PX4_INFO("Output: AUX");
}

void OutputRC::_stream_device_attitude_status() {
    gimbal_device_attitude_status_s attitude_status{};
    attitude_status.timestamp        = hrt_absolute_time();
    attitude_status.target_system    = 0;
    attitude_status.target_component = 0;
    attitude_status.device_flags     = gimbal_device_attitude_status_s::DEVICE_FLAGS_NEUTRAL |
                                   gimbal_device_attitude_status_s::DEVICE_FLAGS_ROLL_LOCK |
                                   gimbal_device_attitude_status_s::DEVICE_FLAGS_PITCH_LOCK |
                                   gimbal_device_attitude_status_s::DEVICE_FLAGS_YAW_LOCK;

    matrix::Eulerf euler(_angle_outputs[0], _angle_outputs[1], _angle_outputs[2]);
    matrix::Quatf  q(euler);
    q.copyTo(attitude_status.q);

    attitude_status.failure_flags = 0;
    _attitude_status_pub.publish(attitude_status);
}

} /* namespace gimbal */
