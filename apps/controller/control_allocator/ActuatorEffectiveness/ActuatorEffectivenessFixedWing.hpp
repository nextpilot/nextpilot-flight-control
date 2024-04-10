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
#include "ActuatorEffectivenessControlSurfaces.hpp"

#include <uORB/topics/normalized_unsigned_setpoint.h>

using namespace nextpilot;

class ActuatorEffectivenessFixedWing : public ModuleParams, public ActuatorEffectiveness {
public:
    ActuatorEffectivenessFixedWing(ModuleParams *parent);
    virtual ~ActuatorEffectivenessFixedWing() = default;

    bool getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) override;

    const char *name() const override {
        return "Fixed Wing";
    }

    void allocateAuxilaryControls(const float dt, int matrix_index, ActuatorVector &actuator_sp) override;

    void updateSetpoint(const matrix::Vector<float, NUM_AXES> &control_sp, int matrix_index,
                        ActuatorVector &actuator_sp, const matrix::Vector<float, NUM_ACTUATORS> &actuator_min,
                        const matrix::Vector<float, NUM_ACTUATORS> &actuator_max) override;

private:
    ActuatorEffectivenessRotors          _rotors;
    ActuatorEffectivenessControlSurfaces _control_surfaces;

    uORB::Subscription _flaps_setpoint_sub{ORB_ID(flaps_setpoint)};
    uORB::Subscription _spoilers_setpoint_sub{ORB_ID(spoilers_setpoint)};

    int _first_control_surface_idx{0}; ///< applies to matrix 1

    uint32_t _forwards_motors_mask{};
};
