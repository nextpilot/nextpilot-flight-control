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
 * @file ActuatorEffectivenessStandardVTOL.hpp
 *
 * Actuator effectiveness for standard VTOL
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#pragma once

#include "ActuatorEffectiveness.hpp"
#include "ActuatorEffectivenessRotors.hpp"
#include "ActuatorEffectivenessControlSurfaces.hpp"

#include <uORB/topics/normalized_unsigned_setpoint.h>

class ActuatorEffectivenessStandardVTOL : public ModuleParams, public ActuatorEffectiveness {
public:
    ActuatorEffectivenessStandardVTOL(ModuleParams *parent);
    virtual ~ActuatorEffectivenessStandardVTOL() = default;

    bool getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) override;

    const char *name() const override {
        return "Standard VTOL";
    }

    int numMatrices() const override {
        return 2;
    }

    void getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_NUM_MATRICES]) const override {
        static_assert(MAX_NUM_MATRICES >= 2, "expecting at least 2 matrices");
        allocation_method_out[0] = AllocationMethod::SEQUENTIAL_DESATURATION;
        allocation_method_out[1] = AllocationMethod::PSEUDO_INVERSE;
    }

    void getNormalizeRPY(bool normalize[MAX_NUM_MATRICES]) const override {
        normalize[0] = true;
        normalize[1] = false;
    }

    void allocateAuxilaryControls(const float dt, int matrix_index, ActuatorVector &actuator_sp) override;

    void updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp, int matrix_index,
                        ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                        const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) override;

    void setFlightPhase(const FlightPhase &flight_phase) override;

private:
    ActuatorEffectivenessRotors          _rotors;
    ActuatorEffectivenessControlSurfaces _control_surfaces;

    uint32_t _upwards_motors_mask{};
    uint32_t _forwards_motors_mask{};

    int _first_control_surface_idx{0}; ///< applies to matrix 1

    uORB::Subscription _flaps_setpoint_sub{ORB_ID(flaps_setpoint)};
    uORB::Subscription _spoilers_setpoint_sub{ORB_ID(spoilers_setpoint)};
};
