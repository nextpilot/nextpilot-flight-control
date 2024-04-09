/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessCustom.hpp"

using namespace matrix;

ActuatorEffectivenessCustom::ActuatorEffectivenessCustom(ModuleParams *parent) :
    ModuleParams(parent), _motors(this), _torque(this) {
}

bool ActuatorEffectivenessCustom::getEffectivenessMatrix(Configuration            &configuration,
                                                         EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    // Motors
    _motors.enablePropellerTorque(false);
    const bool motors_added_successfully = _motors.addActuators(configuration);
    _motors_mask                         = _motors.getMotors();

    // Torque
    const bool torque_added_successfully = _torque.addActuators(configuration);

    return (motors_added_successfully && torque_added_successfully);
}

void ActuatorEffectivenessCustom::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                                 int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                                 const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    stopMaskedMotorsWithZeroThrust(_motors_mask, actuator_sp);
}
