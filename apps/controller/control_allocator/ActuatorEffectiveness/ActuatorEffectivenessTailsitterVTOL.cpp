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
 * @file ActuatorEffectivenessTailsitterVTOL.hpp
 *
 * Actuator effectiveness for tailsitter VTOL
 */

#include "ActuatorEffectivenessTailsitterVTOL.hpp"

using namespace matrix;

ActuatorEffectivenessTailsitterVTOL::ActuatorEffectivenessTailsitterVTOL(ModuleParams *parent) :
    ModuleParams(parent), _mc_rotors(this), _control_surfaces(this) {
    setFlightPhase(FlightPhase::HOVER_FLIGHT);
}

bool ActuatorEffectivenessTailsitterVTOL::getEffectivenessMatrix(Configuration            &configuration,
                                                                 EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    // MC motors
    configuration.selected_matrix = 0;
    // enable MC yaw control if more than 3 rotors
    _mc_rotors.enableYawByDifferentialThrust(_mc_rotors.geometry().num_rotors > 3);
    const bool mc_rotors_added_successfully = _mc_rotors.addActuators(configuration);

    // Control Surfaces
    configuration.selected_matrix          = 1;
    _first_control_surface_idx             = configuration.num_actuators_matrix[configuration.selected_matrix];
    const bool surfaces_added_successfully = _control_surfaces.addActuators(configuration);

    return (mc_rotors_added_successfully && surfaces_added_successfully);
}

void ActuatorEffectivenessTailsitterVTOL::allocateAuxilaryControls(const float dt, int matrix_index,
                                                                   ActuatorVector &actuator_sp) {
    if (matrix_index == 1) {
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
}

void ActuatorEffectivenessTailsitterVTOL::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                                         int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                                         const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    if (matrix_index == 0) {
        stopMaskedMotorsWithZeroThrust(_forwards_motors_mask, actuator_sp);
    }
}

void ActuatorEffectivenessTailsitterVTOL::setFlightPhase(const FlightPhase &flight_phase) {
    if (_flight_phase == flight_phase) {
        return;
    }

    ActuatorEffectiveness::setFlightPhase(flight_phase);

    // update stopped motors
    switch (flight_phase) {
    case FlightPhase::FORWARD_FLIGHT:
        _forwards_motors_mask = _mc_rotors.getUpwardsMotors(); // allocation frame they stay upwards
        break;

    case FlightPhase::HOVER_FLIGHT:
    case FlightPhase::TRANSITION_FF_TO_HF:
    case FlightPhase::TRANSITION_HF_TO_FF:
        _forwards_motors_mask = 0;
        _stopped_motors_mask  = 0;
        break;
    }
}
