/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessUUV.hpp"

using namespace matrix;

ActuatorEffectivenessUUV::ActuatorEffectivenessUUV(ModuleParams *parent) :
    ModuleParams(parent),
    _rotors(this) {
}

bool ActuatorEffectivenessUUV::getEffectivenessMatrix(Configuration            &configuration,
                                                      EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    // Motors
    const bool rotors_added_successfully = _rotors.addActuators(configuration);
    _motors_mask                         = _rotors.getMotors();

    return rotors_added_successfully;
}

void ActuatorEffectivenessUUV::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                              int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                              const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    stopMaskedMotorsWithZeroThrust(_motors_mask, actuator_sp);
}
