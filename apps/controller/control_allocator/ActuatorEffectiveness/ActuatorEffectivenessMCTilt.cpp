/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessMCTilt.hpp"

using namespace matrix;

ActuatorEffectivenessMCTilt::ActuatorEffectivenessMCTilt(ModuleParams *parent) :
    ModuleParams(parent),
    _mc_rotors(this, ActuatorEffectivenessRotors::AxisConfiguration::FixedUpwards, true),
    _tilts(this) {
}

bool ActuatorEffectivenessMCTilt::getEffectivenessMatrix(Configuration            &configuration,
                                                         EffectivenessUpdateReason external_update) {
    if (external_update == EffectivenessUpdateReason::NO_EXTERNAL_UPDATE) {
        return false;
    }

    // MC motors
    _mc_rotors.enableYawByDifferentialThrust(!_tilts.hasYawControl());
    const bool rotors_added_successfully = _mc_rotors.addActuators(configuration);

    // Tilts
    int first_tilt_idx = configuration.num_actuators_matrix[0];
    _tilts.updateTorqueSign(_mc_rotors.geometry());
    const bool tilts_added_successfully = _tilts.addActuators(configuration);

    // Set offset such that tilts point upwards when control input == 0 (trim is 0 if min_angle == -max_angle).
    // Note that we don't set configuration.trim here, because in the case of trim == +-1, yaw is always saturated
    // and reduced to 0 with the sequential desaturation method. Instead we add it after.
    _tilt_offsets.setZero();

    for (int i = 0; i < _tilts.count(); ++i) {
        float delta_angle = _tilts.config(i).max_angle - _tilts.config(i).min_angle;

        if (delta_angle > FLT_EPSILON) {
            float trim                        = -1.f - 2.f * _tilts.config(i).min_angle / delta_angle;
            _tilt_offsets(first_tilt_idx + i) = trim;
        }
    }

    return (rotors_added_successfully && tilts_added_successfully);
}

void ActuatorEffectivenessMCTilt::updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp,
                                                 int matrix_index, ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                                                 const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) {
    actuator_sp += _tilt_offsets;
    // TODO: dynamic matrix update
}
