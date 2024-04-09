/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessMultirotor.hpp"

using namespace matrix;

ActuatorEffectivenessMultirotor::ActuatorEffectivenessMultirotor(ModuleParams *parent) :
    ModuleParams(parent),
    _mc_rotors(this) {
}

bool ActuatorEffectivenessMultirotor::getEffectivenessMatrix(Configuration            &configuration,
                                                             EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    // Motors
    const bool rotors_added_successfully = _mc_rotors.addActuators(configuration);

    return rotors_added_successfully;
}
