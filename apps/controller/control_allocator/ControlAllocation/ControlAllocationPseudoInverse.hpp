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
 * @file ControlAllocationPseudoInverse.hpp
 *
 * Simple Control Allocation Algorithm
 *
 * It computes the pseudo-inverse of the effectiveness matrix
 * Actuator saturation is handled by simple clipping, do not
 * expect good performance in case of actuator saturation.
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#pragma once

#include "ControlAllocation.hpp"

class ControlAllocationPseudoInverse : public ControlAllocation {
public:
    ControlAllocationPseudoInverse()          = default;
    virtual ~ControlAllocationPseudoInverse() = default;

    void allocate() override;
    void setEffectivenessMatrix(const matrix::Matrix<float, NUM_AXES, NUM_ACTUATORS> &effectiveness,
                                const ActuatorVector &actuator_trim, const ActuatorVector &linearization_point, int num_actuators,
                                bool update_normalization_scale) override;

protected:
    matrix::Matrix<float, NUM_ACTUATORS, NUM_AXES> _mix;

    bool _mix_update_needed{false};

    /**
     * Recalculate pseudo inverse if required.
     *
     */
    void updatePseudoInverse();

private:
    void normalizeControlAllocationMatrix();
    void updateControlAllocationMatrixScale();
    bool _normalization_needs_update{false};
};
