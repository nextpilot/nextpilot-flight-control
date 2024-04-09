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
 * @file ControlAllocation.cpp
 *
 * Interface for Control Allocation Algorithms
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#include "ControlAllocation.hpp"

ControlAllocation::ControlAllocation() {
    _control_allocation_scale.setAll(1.f);
    _actuator_min.setAll(0.f);
    _actuator_max.setAll(1.f);
}

void ControlAllocation::setEffectivenessMatrix(
    const matrix::Matrix<float, ControlAllocation::NUM_AXES, ControlAllocation::NUM_ACTUATORS> &effectiveness,
    const ActuatorVector &actuator_trim, const ActuatorVector &linearization_point, int num_actuators,
    bool update_normalization_scale) {
    _effectiveness                             = effectiveness;
    ActuatorVector linearization_point_clipped = linearization_point;
    clipActuatorSetpoint(linearization_point_clipped);
    _actuator_trim = actuator_trim + linearization_point_clipped;
    clipActuatorSetpoint(_actuator_trim);
    _num_actuators = num_actuators;
    _control_trim  = _effectiveness * linearization_point_clipped;
}

void ControlAllocation::setActuatorSetpoint(
    const matrix::Vector<float, ControlAllocation::NUM_ACTUATORS> &actuator_sp) {
    // Set actuator setpoint
    _actuator_sp = actuator_sp;

    // Clip
    clipActuatorSetpoint(_actuator_sp);
}

void ControlAllocation::clipActuatorSetpoint(matrix::Vector<float, ControlAllocation::NUM_ACTUATORS> &actuator) const {
    for (int i = 0; i < _num_actuators; i++) {
        if (_actuator_max(i) < _actuator_min(i)) {
            actuator(i) = _actuator_trim(i);

        } else if (actuator(i) < _actuator_min(i)) {
            actuator(i) = _actuator_min(i);

        } else if (actuator(i) > _actuator_max(i)) {
            actuator(i) = _actuator_max(i);
        }
    }
}

matrix::Vector<float, ControlAllocation::NUM_ACTUATORS>
ControlAllocation::normalizeActuatorSetpoint(const matrix::Vector<float, ControlAllocation::NUM_ACTUATORS> &actuator)
    const {
    matrix::Vector<float, ControlAllocation::NUM_ACTUATORS> actuator_normalized;

    for (int i = 0; i < _num_actuators; i++) {
        if (_actuator_min(i) < _actuator_max(i)) {
            actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

        } else {
            actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
        }
    }

    return actuator_normalized;
}

void ControlAllocation::applySlewRateLimit(float dt) {
    for (int i = 0; i < _num_actuators; i++) {
        if (_actuator_slew_rate_limit(i) > FLT_EPSILON) {
            float delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / _actuator_slew_rate_limit(i);
            float delta_sp     = _actuator_sp(i) - _prev_actuator_sp(i);

            if (delta_sp > delta_sp_max) {
                _actuator_sp(i) = _prev_actuator_sp(i) + delta_sp_max;

            } else if (delta_sp < -delta_sp_max) {
                _actuator_sp(i) = _prev_actuator_sp(i) - delta_sp_max;
            }
        }
    }
}
