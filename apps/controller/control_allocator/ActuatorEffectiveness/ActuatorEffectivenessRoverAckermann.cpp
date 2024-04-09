/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessRoverAckermann.hpp"
#include <ControlAllocation/ControlAllocation.hpp>

using namespace matrix;

bool ActuatorEffectivenessRoverAckermann::getEffectivenessMatrix(Configuration            &configuration,
                                                                 EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    configuration.addActuator(ActuatorType::MOTORS, Vector3f{}, Vector3f{1.f, 0.f, 0.f});
    _motors_mask = 1u << 0;
    configuration.addActuator(ActuatorType::SERVOS, Vector3f{0.f, 0.f, 1.f}, Vector3f{});
    return true;
}

void ActuatorEffectivenessRoverAckermann::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                                         int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                                         const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    stopMaskedMotorsWithZeroThrust(_motors_mask, actuator_sp);
}
