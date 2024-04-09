/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include "ActuatorEffectivenessHelicopter.hpp"

using namespace matrix;

TEST(ActuatorEffectivenessHelicopterTest, ThrottleCurve) {
    // Disable autosaving parameters to avoid busy loop in param_set()
    param_control_autosave(false);

    // Set parameters for a predefined throttle curve
    float   values[5] = {0.f, .3f, .6f, .8f, 1.f};
    param_t param;

    for (int i = 0; i < 5; i++) {
        char buffer[17];
        snprintf(buffer, sizeof(buffer), "CA_HELI_THR_C%u", i);
        param = param_find(buffer);
        param_set(param, &values[i]);
    }

    ActuatorEffectivenessHelicopter helicopter(nullptr);
    // run getEffectivenessMatrix with empty configuration to correctly initialize _first_swash_plate_servo_index
    ActuatorEffectiveness::Configuration configuration{};
    EffectivenessUpdateReason            external_update = EffectivenessUpdateReason::MOTOR_ACTIVATION_UPDATE;
    helicopter.getEffectivenessMatrix(configuration, external_update);

    Vector<float, 6>                      control_sp{};
    ActuatorEffectiveness::ActuatorVector actuator_sp{};
    ActuatorEffectiveness::ActuatorVector actuator_min{};
    actuator_min.setAll(0.f);
    ActuatorEffectiveness::ActuatorVector actuator_max{};
    actuator_max.setAll(1.f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = 0.1f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), 0.f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = 0.f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), 0.f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.125f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .15f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.25f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .3f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.375f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .45f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.5f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .6f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.625f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .7f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.75f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .8f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -.875f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), .9f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -1.f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), 1.f);

    control_sp(ActuatorEffectiveness::ControlAxis::THRUST_Z) = -1.1f;
    helicopter.updateSetpoint(control_sp, 0, actuator_sp, actuator_min, actuator_max);
    EXPECT_FLOAT_EQ(actuator_sp(0), 1.f);
}
