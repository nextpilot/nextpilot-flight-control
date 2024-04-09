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
 * @file ControlAllocationPseudoInverse.hpp
 *
 * Simple Control Allocation Algorithm
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#include "ControlAllocationPseudoInverse.hpp"

void ControlAllocationPseudoInverse::setEffectivenessMatrix(
    const matrix::Matrix<float, ControlAllocation::NUM_AXES, ControlAllocation::NUM_ACTUATORS> &effectiveness,
    const ActuatorVector &actuator_trim, const ActuatorVector &linearization_point, int num_actuators,
    bool update_normalization_scale) {
    ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, num_actuators,
                                              update_normalization_scale);
    _mix_update_needed          = true;
    _normalization_needs_update = update_normalization_scale;
}

void ControlAllocationPseudoInverse::updatePseudoInverse() {
    if (_mix_update_needed) {
        matrix::geninv(_effectiveness, _mix);

        if (_normalization_needs_update && !_had_actuator_failure) {
            updateControlAllocationMatrixScale();
            _normalization_needs_update = false;
        }

        normalizeControlAllocationMatrix();
        _mix_update_needed = false;
    }
}

void ControlAllocationPseudoInverse::updateControlAllocationMatrixScale() {
    // Same scale on roll and pitch
    if (_normalize_rpy) {
        int num_non_zero_roll_torque  = 0;
        int num_non_zero_pitch_torque = 0;

        for (int i = 0; i < _num_actuators; i++) {
            if (fabsf(_mix(i, 0)) > 1e-3f) {
                ++num_non_zero_roll_torque;
            }

            if (fabsf(_mix(i, 1)) > 1e-3f) {
                ++num_non_zero_pitch_torque;
            }
        }

        float roll_norm_scale = 1.f;

        if (num_non_zero_roll_torque > 0) {
            roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (num_non_zero_roll_torque / 2.f));
        }

        float pitch_norm_scale = 1.f;

        if (num_non_zero_pitch_torque > 0) {
            pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (num_non_zero_pitch_torque / 2.f));
        }

        _control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
        _control_allocation_scale(1) = _control_allocation_scale(0);

        // Scale yaw separately
        _control_allocation_scale(2) = _mix.col(2).max();

    } else {
        _control_allocation_scale(0) = 1.f;
        _control_allocation_scale(1) = 1.f;
        _control_allocation_scale(2) = 1.f;
    }

    // Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
    // (for tilted actuators)
    _control_allocation_scale(THRUST_Z) = 1.f;

    for (int axis_idx = 2; axis_idx >= 0; --axis_idx) {
        int   num_non_zero_thrust = 0;
        float norm_sum            = 0.f;

        for (int i = 0; i < _num_actuators; i++) {
            float norm = fabsf(_mix(i, 3 + axis_idx));
            norm_sum += norm;

            if (norm > FLT_EPSILON) {
                ++num_non_zero_thrust;
            }
        }

        if (num_non_zero_thrust > 0) {
            _control_allocation_scale(3 + axis_idx) = norm_sum / num_non_zero_thrust;

        } else {
            _control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
        }
    }
}

void ControlAllocationPseudoInverse::normalizeControlAllocationMatrix() {
    if (_control_allocation_scale(0) > FLT_EPSILON) {
        _mix.col(0) /= _control_allocation_scale(0);
        _mix.col(1) /= _control_allocation_scale(1);
    }

    if (_control_allocation_scale(2) > FLT_EPSILON) {
        _mix.col(2) /= _control_allocation_scale(2);
    }

    if (_control_allocation_scale(3) > FLT_EPSILON) {
        _mix.col(3) /= _control_allocation_scale(3);
        _mix.col(4) /= _control_allocation_scale(4);
        _mix.col(5) /= _control_allocation_scale(5);
    }

    // Set all the small elements to 0 to avoid issues
    // in the control allocation algorithms
    for (int i = 0; i < _num_actuators; i++) {
        for (int j = 0; j < NUM_AXES; j++) {
            if (fabsf(_mix(i, j)) < 1e-3f) {
                _mix(i, j) = 0.f;
            }
        }
    }
}

void ControlAllocationPseudoInverse::allocate() {
    // Compute new gains if needed
    updatePseudoInverse();

    _prev_actuator_sp = _actuator_sp;

    // Allocate
    _actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}
