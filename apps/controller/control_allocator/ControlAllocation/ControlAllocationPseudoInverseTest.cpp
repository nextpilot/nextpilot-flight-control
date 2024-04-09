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
 * @file ControlAllocationTest.cpp
 *
 * Tests for Control Allocation Algorithms
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#include <gtest/gtest.h>
#include <ControlAllocationPseudoInverse.hpp>

using namespace matrix;

TEST(ControlAllocationTest, AllZeroCase) {
    ControlAllocationPseudoInverse method;

    matrix::Vector<float, 6>     control_sp;
    matrix::Vector<float, 6>     control_allocated;
    matrix::Vector<float, 6>     control_allocated_expected;
    matrix::Matrix<float, 6, 16> effectiveness;
    matrix::Vector<float, 16>    actuator_sp;
    matrix::Vector<float, 16>    actuator_trim;
    matrix::Vector<float, 16>    linearization_point;
    matrix::Vector<float, 16>    actuator_sp_expected;

    method.setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, 16, false);
    method.setControlSetpoint(control_sp);
    method.allocate();
    method.clipActuatorSetpoint();
    actuator_sp                = method.getActuatorSetpoint();
    control_allocated_expected = method.getAllocatedControl();

    EXPECT_EQ(actuator_sp, actuator_sp_expected);
    EXPECT_EQ(control_allocated, control_allocated_expected);
}
