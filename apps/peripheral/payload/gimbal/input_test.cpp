/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "input_test.h"


#include <ulog/log.h>
#include <matrix/matrix/math.hpp>
#include <mathlib/mathlib.h>

namespace gimbal {

InputTest::InputTest(Parameters &parameters) :
    InputBase(parameters) {
}

InputTest::UpdateResult InputTest::update(unsigned int timeout_ms, ControlData &control_data, bool already_active) {
    if (!_has_been_set.load()) {
        return UpdateResult::NoUpdate;
    }

    control_data.type = ControlData::Type::Angle;

    control_data.type_data.angle.frames[0] = ControlData::TypeData::TypeAngle::Frame::AngleAbsoluteFrame;
    control_data.type_data.angle.frames[1] = ControlData::TypeData::TypeAngle::Frame::AngleAbsoluteFrame;
    control_data.type_data.angle.frames[2] = ControlData::TypeData::TypeAngle::Frame::AngleBodyFrame;

    matrix::Eulerf euler(
        math::radians((float)_roll_deg),
        math::radians((float)_pitch_deg),
        math::radians((float)_yaw_deg));
    matrix::Quatf q(euler);

    q.copyTo(control_data.type_data.angle.q);

    control_data.type_data.angle.angular_velocity[0] = NAN;
    control_data.type_data.angle.angular_velocity[1] = NAN;
    control_data.type_data.angle.angular_velocity[2] = NAN;

    // For testing we mark ourselves as in control.
    control_data.sysid_primary_control  = _parameters.mav_sysid;
    control_data.compid_primary_control = _parameters.mav_compid;

    _has_been_set.store(false);
    return UpdateResult::UpdatedActive;
}

int InputTest::initialize() {
    return 0;
}

void InputTest::print_status() const {
    PX4_INFO("Input: Test");
    PX4_INFO_RAW("  roll : % 3d deg\n", _roll_deg);
    PX4_INFO_RAW("  pitch: % 3d deg\n", _pitch_deg);
    PX4_INFO_RAW("  yaw  : % 3d deg\n", _yaw_deg);
}

void InputTest::set_test_input(int roll_deg, int pitch_deg, int yaw_deg) {
    _roll_deg  = roll_deg;
    _pitch_deg = pitch_deg;
    _yaw_deg   = yaw_deg;

    _has_been_set.store(true);
}

} /* namespace gimbal */
