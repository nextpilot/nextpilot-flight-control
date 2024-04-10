/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "ActuatorEffectiveness"

#include "ActuatorEffectiveness.hpp"
#include "ControlAllocation/ControlAllocation.hpp"
#include <rtdbg.h>

// #include <px4_platform_common/log.h>

int ActuatorEffectiveness::Configuration::addActuator(ActuatorType type, const matrix::Vector3f &torque,
                                                      const matrix::Vector3f &thrust) {
    int actuator_idx = num_actuators_matrix[selected_matrix];

    if (actuator_idx >= NUM_ACTUATORS) {
        PX4_ERR("Too many actuators");
        return -1;
    }

    if ((int)type < (int)ActuatorType::COUNT - 1 && num_actuators[(int)type + 1] > 0) {
        PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
        return -1;
    }

    effectiveness_matrices[selected_matrix](ControlAllocation::ControlAxis::ROLL, actuator_idx)     = torque(0);
    effectiveness_matrices[selected_matrix](ControlAllocation::ControlAxis::PITCH, actuator_idx)    = torque(1);
    effectiveness_matrices[selected_matrix](ControlAllocation::ControlAxis::YAW, actuator_idx)      = torque(2);
    effectiveness_matrices[selected_matrix](ControlAllocation::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
    effectiveness_matrices[selected_matrix](ControlAllocation::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
    effectiveness_matrices[selected_matrix](ControlAllocation::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
    matrix_selection_indexes[totalNumActuators()]                                                   = selected_matrix;
    ++num_actuators[(int)type];
    return num_actuators_matrix[selected_matrix]++;
}

void ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, int count) {
    int total_count = totalNumActuators();

    for (int i = 0; i < count; ++i) {
        matrix_selection_indexes[i + total_count] = selected_matrix;
    }

    num_actuators[(int)type] += count;
    num_actuators_matrix[selected_matrix] += count;
}

int ActuatorEffectiveness::Configuration::totalNumActuators() const {
    int total_count = 0;

    for (int i = 0; i < MAX_NUM_MATRICES; ++i) {
        total_count += num_actuators_matrix[i];
    }

    return total_count;
}

void ActuatorEffectiveness::stopMaskedMotorsWithZeroThrust(uint32_t stoppable_motors_mask, ActuatorVector &actuator_sp) {
    for (int actuator_idx = 0; actuator_idx < NUM_ACTUATORS; actuator_idx++) {
        const uint32_t motor_mask = (1u << actuator_idx);

        if (stoppable_motors_mask & motor_mask) {
            if (fabsf(actuator_sp(actuator_idx)) < .02f) {
                _stopped_motors_mask |= motor_mask;

            } else {
                _stopped_motors_mask &= ~motor_mask;
            }
        }
    }
}
