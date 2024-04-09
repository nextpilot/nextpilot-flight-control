/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "ActuatorEffectiveness.hpp"
#include "ActuatorEffectivenessRotors.hpp"
#include "ActuatorEffectivenessTilts.hpp"

class ActuatorEffectivenessMCTilt : public ModuleParams, public ActuatorEffectiveness {
public:
    ActuatorEffectivenessMCTilt(ModuleParams *parent);
    virtual ~ActuatorEffectivenessMCTilt() = default;

    bool getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) override;

    void getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_NUM_MATRICES]) const override {
        allocation_method_out[0] = AllocationMethod::SEQUENTIAL_DESATURATION;
    }

    void getNormalizeRPY(bool normalize[MAX_NUM_MATRICES]) const override {
        normalize[0] = true;
    }

    void updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp, int matrix_index,
                        ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                        const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) override;

    const char *name() const override {
        return "MC Tilt";
    }

protected:
    ActuatorVector              _tilt_offsets;
    ActuatorEffectivenessRotors _mc_rotors;
    ActuatorEffectivenessTilts  _tilts;
};
