/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessFixedWing.hpp"
#include <ControlAllocation/ControlAllocation.hpp>

using namespace matrix;

ActuatorEffectivenessFixedWing::ActuatorEffectivenessFixedWing(ModuleParams *parent) :
    ModuleParams(parent), _rotors(this, ActuatorEffectivenessRotors::AxisConfiguration::FixedForward),
    _control_surfaces(this) {
}

bool ActuatorEffectivenessFixedWing::getEffectivenessMatrix(Configuration            &configuration,
                                                            EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    // Motors
    _rotors.enablePropellerTorque(false);
    const bool rotors_added_successfully = _rotors.addActuators(configuration);
    _forwards_motors_mask                = _rotors.getForwardsMotors();

    // Control Surfaces
    _first_control_surface_idx             = configuration.num_actuators_matrix[0];
    const bool surfaces_added_successfully = _control_surfaces.addActuators(configuration);

    return (rotors_added_successfully && surfaces_added_successfully);
}

void ActuatorEffectivenessFixedWing::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                                    int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                                    const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    stopMaskedMotorsWithZeroThrust(_forwards_motors_mask, actuator_sp);
}

void ActuatorEffectivenessFixedWing::allocateAuxilaryControls(const float dt, int matrix_index,
                                                              ActuatorVector &actuator_sp) {
    // apply flaps
    normalized_unsigned_setpoint_s flaps_setpoint;

    if (_flaps_setpoint_sub.copy(&flaps_setpoint)) {
        _control_surfaces.applyFlaps(flaps_setpoint.normalized_setpoint, _first_control_surface_idx, dt, actuator_sp);
    }

    // apply spoilers
    normalized_unsigned_setpoint_s spoilers_setpoint;

    if (_spoilers_setpoint_sub.copy(&spoilers_setpoint)) {
        _control_surfaces.applySpoilers(spoilers_setpoint.normalized_setpoint, _first_control_surface_idx, dt, actuator_sp);
    }
}
