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
 * @file ActuatorEffectivenessRotors.cpp
 *
 * Tests for Control Allocation Algorithms
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#include <gtest/gtest.h>
#include "ActuatorEffectivenessRotors.hpp"

using namespace matrix;

TEST(ActuatorEffectivenessRotors, AllZeroCase) {
    // Quad wide geometry
    ActuatorEffectivenessRotors::Geometry geometry = {};
    geometry.rotors[0].position(0)                 = 1.0f;
    geometry.rotors[0].position(1)                 = 1.0f;
    geometry.rotors[0].position(2)                 = 0.0f;
    geometry.rotors[0].axis(0)                     = 0.0f;
    geometry.rotors[0].axis(1)                     = 0.0f;
    geometry.rotors[0].axis(2)                     = -1.0f;
    geometry.rotors[0].thrust_coef                 = 1.0f;
    geometry.rotors[0].moment_ratio                = 0.05f;

    geometry.rotors[1].position(0)  = -1.0f;
    geometry.rotors[1].position(1)  = -1.0f;
    geometry.rotors[1].position(2)  = 0.0f;
    geometry.rotors[1].axis(0)      = 0.0f;
    geometry.rotors[1].axis(1)      = 0.0f;
    geometry.rotors[1].axis(2)      = -1.0f;
    geometry.rotors[1].thrust_coef  = 1.0f;
    geometry.rotors[1].moment_ratio = 0.05f;

    geometry.rotors[2].position(0)  = 1.0f;
    geometry.rotors[2].position(1)  = -1.0f;
    geometry.rotors[2].position(2)  = 0.0f;
    geometry.rotors[2].axis(0)      = 0.0f;
    geometry.rotors[2].axis(1)      = 0.0f;
    geometry.rotors[2].axis(2)      = -1.0f;
    geometry.rotors[2].thrust_coef  = 1.0f;
    geometry.rotors[2].moment_ratio = -0.05f;

    geometry.rotors[3].position(0)  = -1.0f;
    geometry.rotors[3].position(1)  = 1.0f;
    geometry.rotors[3].position(2)  = 0.0f;
    geometry.rotors[3].axis(0)      = 0.0f;
    geometry.rotors[3].axis(1)      = 0.0f;
    geometry.rotors[3].axis(2)      = -1.0f;
    geometry.rotors[3].thrust_coef  = 1.0f;
    geometry.rotors[3].moment_ratio = -0.05f;

    geometry.num_rotors = 4;

    ActuatorEffectiveness::EffectivenessMatrix effectiveness;
    effectiveness.setZero();
    ActuatorEffectivenessRotors::computeEffectivenessMatrix(geometry, effectiveness);

    const float expected[ActuatorEffectiveness::NUM_AXES][ActuatorEffectiveness::NUM_ACTUATORS] = {
        {-1.0f, 1.0f, 1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
        {1.0f, -1.0f, 1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
        {0.05f, 0.05f, -0.05f, -0.05f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
        {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
        {-1.0f, -1.0f, -1.0f, -1.0f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}};
    ActuatorEffectiveness::EffectivenessMatrix effectiveness_expected(expected);

    EXPECT_EQ(effectiveness, effectiveness_expected);
}

TEST(ActuatorEffectivenessRotors, Tilt) {
    Vector3f axis_expected{0.f, 0.f, -1.f};
    Vector3f axis = ActuatorEffectivenessRotors::tiltedAxis(0.f, 0.f);
    EXPECT_EQ(axis, axis_expected);

    axis_expected = Vector3f{1.f, 0.f, 0.f};
    axis          = ActuatorEffectivenessRotors::tiltedAxis(M_PI_F / 2.f, 0.f);
    EXPECT_EQ(axis, axis_expected);

    axis_expected = Vector3f{1.f / sqrtf(2.f), 0.f, -1.f / sqrtf(2.f)};
    axis          = ActuatorEffectivenessRotors::tiltedAxis(M_PI_F / 2.f / 2.f, 0.f);
    EXPECT_EQ(axis, axis_expected);

    axis_expected = Vector3f{-1.f, 0.f, 0.f};
    axis          = ActuatorEffectivenessRotors::tiltedAxis(-M_PI_F / 2.f, 0.f);
    EXPECT_EQ(axis, axis_expected);

    axis_expected = Vector3f{0.f, 0.f, -1.f};
    axis          = ActuatorEffectivenessRotors::tiltedAxis(0.f, M_PI_F / 2.f);
    EXPECT_EQ(axis, axis_expected);

    axis_expected = Vector3f{0.f, 1.f, 0.f};
    axis          = ActuatorEffectivenessRotors::tiltedAxis(M_PI_F / 2.f, M_PI_F / 2.f);
    EXPECT_EQ(axis, axis_expected);

    axis_expected = Vector3f{0.f, -1.f, 0.f};
    axis          = ActuatorEffectivenessRotors::tiltedAxis(-M_PI_F / 2.f, M_PI_F / 2.f);
    EXPECT_EQ(axis, axis_expected);
}
