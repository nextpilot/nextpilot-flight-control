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
 * @file ControlAllocationSequentialDesaturation.cpp
 *
 * @author Roman Bapst <bapstroman@gmail.com>
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#include "ControlAllocationSequentialDesaturation.hpp"

void ControlAllocationSequentialDesaturation::allocate() {
    // Compute new gains if needed
    updatePseudoInverse();

    _prev_actuator_sp = _actuator_sp;

    switch (_param_mc_airmode.get()) {
    case 1:
        mixAirmodeRP();
        break;

    case 2:
        mixAirmodeRPY();
        break;

    default:
        mixAirmodeDisabled();
        break;
    }
}

void ControlAllocationSequentialDesaturation::desaturateActuators(
    ActuatorVector       &actuator_sp,
    const ActuatorVector &desaturation_vector, bool increase_only) {
    float gain = computeDesaturationGain(desaturation_vector, actuator_sp);

    if (increase_only && gain < 0.f) {
        return;
    }

    for (int i = 0; i < _num_actuators; i++) {
        actuator_sp(i) += gain * desaturation_vector(i);
    }

    gain = 0.5f * computeDesaturationGain(desaturation_vector, actuator_sp);

    for (int i = 0; i < _num_actuators; i++) {
        actuator_sp(i) += gain * desaturation_vector(i);
    }
}

float ControlAllocationSequentialDesaturation::computeDesaturationGain(const ActuatorVector &desaturation_vector,
                                                                       const ActuatorVector &actuator_sp) {
    float k_min = 0.f;
    float k_max = 0.f;

    for (int i = 0; i < _num_actuators; i++) {
        // Do not use try to desaturate using an actuator with weak effectiveness to avoid large desaturation gains
        if (fabsf(desaturation_vector(i)) < 0.2f) {
            continue;
        }

        if (actuator_sp(i) < _actuator_min(i)) {
            float k = (_actuator_min(i) - actuator_sp(i)) / desaturation_vector(i);

            if (k < k_min) { k_min = k; }

            if (k > k_max) { k_max = k; }
        }

        if (actuator_sp(i) > _actuator_max(i)) {
            float k = (_actuator_max(i) - actuator_sp(i)) / desaturation_vector(i);

            if (k < k_min) { k_min = k; }

            if (k > k_max) { k_max = k; }
        }
    }

    // Reduce the saturation as much as possible
    return k_min + k_max;
}

void ControlAllocationSequentialDesaturation::mixAirmodeRP() {
    // Airmode for roll and pitch, but not yaw

    // Mix without yaw
    ActuatorVector thrust_z;

    for (int i = 0; i < _num_actuators; i++) {
        _actuator_sp(i) = _actuator_trim(i) +
                          _mix(i, ControlAxis::ROLL) * (_control_sp(ControlAxis::ROLL) - _control_trim(ControlAxis::ROLL)) +
                          _mix(i, ControlAxis::PITCH) * (_control_sp(ControlAxis::PITCH) - _control_trim(ControlAxis::PITCH)) +
                          _mix(i, ControlAxis::THRUST_X) * (_control_sp(ControlAxis::THRUST_X) - _control_trim(ControlAxis::THRUST_X)) +
                          _mix(i, ControlAxis::THRUST_Y) * (_control_sp(ControlAxis::THRUST_Y) - _control_trim(ControlAxis::THRUST_Y)) +
                          _mix(i, ControlAxis::THRUST_Z) * (_control_sp(ControlAxis::THRUST_Z) - _control_trim(ControlAxis::THRUST_Z));
        thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
    }

    desaturateActuators(_actuator_sp, thrust_z);

    // Mix yaw independently
    mixYaw();
}

void ControlAllocationSequentialDesaturation::mixAirmodeRPY() {
    // Airmode for roll, pitch and yaw

    // Do full mixing
    ActuatorVector thrust_z;
    ActuatorVector yaw;

    for (int i = 0; i < _num_actuators; i++) {
        _actuator_sp(i) = _actuator_trim(i) +
                          _mix(i, ControlAxis::ROLL) * (_control_sp(ControlAxis::ROLL) - _control_trim(ControlAxis::ROLL)) +
                          _mix(i, ControlAxis::PITCH) * (_control_sp(ControlAxis::PITCH) - _control_trim(ControlAxis::PITCH)) +
                          _mix(i, ControlAxis::YAW) * (_control_sp(ControlAxis::YAW) - _control_trim(ControlAxis::YAW)) +
                          _mix(i, ControlAxis::THRUST_X) * (_control_sp(ControlAxis::THRUST_X) - _control_trim(ControlAxis::THRUST_X)) +
                          _mix(i, ControlAxis::THRUST_Y) * (_control_sp(ControlAxis::THRUST_Y) - _control_trim(ControlAxis::THRUST_Y)) +
                          _mix(i, ControlAxis::THRUST_Z) * (_control_sp(ControlAxis::THRUST_Z) - _control_trim(ControlAxis::THRUST_Z));
        thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
        yaw(i)      = _mix(i, ControlAxis::YAW);
    }

    desaturateActuators(_actuator_sp, thrust_z);

    // Unsaturate yaw (in case upper and lower bounds are exceeded)
    // to prioritize roll/pitch over yaw.
    desaturateActuators(_actuator_sp, yaw);
}

void ControlAllocationSequentialDesaturation::mixAirmodeDisabled() {
    // Airmode disabled: never allow to increase the thrust to unsaturate a motor

    // Mix without yaw
    ActuatorVector thrust_z;
    ActuatorVector roll;
    ActuatorVector pitch;

    for (int i = 0; i < _num_actuators; i++) {
        _actuator_sp(i) = _actuator_trim(i) +
                          _mix(i, ControlAxis::ROLL) * (_control_sp(ControlAxis::ROLL) - _control_trim(ControlAxis::ROLL)) +
                          _mix(i, ControlAxis::PITCH) * (_control_sp(ControlAxis::PITCH) - _control_trim(ControlAxis::PITCH)) +
                          _mix(i, ControlAxis::THRUST_X) * (_control_sp(ControlAxis::THRUST_X) - _control_trim(ControlAxis::THRUST_X)) +
                          _mix(i, ControlAxis::THRUST_Y) * (_control_sp(ControlAxis::THRUST_Y) - _control_trim(ControlAxis::THRUST_Y)) +
                          _mix(i, ControlAxis::THRUST_Z) * (_control_sp(ControlAxis::THRUST_Z) - _control_trim(ControlAxis::THRUST_Z));
        thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
        roll(i)     = _mix(i, ControlAxis::ROLL);
        pitch(i)    = _mix(i, ControlAxis::PITCH);
    }

    // only reduce thrust
    desaturateActuators(_actuator_sp, thrust_z, true);

    // Reduce roll/pitch acceleration if needed to unsaturate
    desaturateActuators(_actuator_sp, roll);
    desaturateActuators(_actuator_sp, pitch);

    // Mix yaw independently
    mixYaw();
}

void ControlAllocationSequentialDesaturation::mixYaw() {
    // Add yaw to outputs
    ActuatorVector yaw;
    ActuatorVector thrust_z;

    for (int i = 0; i < _num_actuators; i++) {
        _actuator_sp(i) += _mix(i, ControlAxis::YAW) * (_control_sp(ControlAxis::YAW) - _control_trim(ControlAxis::YAW));
        yaw(i)      = _mix(i, ControlAxis::YAW);
        thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
    }

    // Change yaw acceleration to unsaturate the outputs if needed (do not change roll/pitch),
    // and allow some yaw response at maximum thrust
    ActuatorVector max_prev = _actuator_max;
    _actuator_max += (_actuator_max - _actuator_min) * 0.15f;
    desaturateActuators(_actuator_sp, yaw);
    _actuator_max = max_prev;

    // reduce thrust only
    desaturateActuators(_actuator_sp, thrust_z, true);
}

void ControlAllocationSequentialDesaturation::updateParameters() {
    updateParams();
}
