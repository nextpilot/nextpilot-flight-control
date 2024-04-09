/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessRoverDifferential.hpp"
#include <ControlAllocation/ControlAllocation.hpp>

using namespace matrix;

bool ActuatorEffectivenessRoverDifferential::getEffectivenessMatrix(Configuration            &configuration,
                                                                    EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    configuration.addActuator(ActuatorType::MOTORS, Vector3f{0.f, 0.f, 0.5f}, Vector3f{0.5f, 0.f, 0.f});
    configuration.addActuator(ActuatorType::MOTORS, Vector3f{0.f, 0.f, -0.5f}, Vector3f{0.5f, 0.f, 0.f});
    _motors_mask = (1u << 0) | (1u << 1);
    return true;
}

void ActuatorEffectivenessRoverDifferential::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                                            int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                                            const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    stopMaskedMotorsWithZeroThrust(_motors_mask, actuator_sp);
}
