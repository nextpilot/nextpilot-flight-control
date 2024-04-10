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

#include <module_params.hpp>
#include <slew_rate/SlewRate.hpp>

static constexpr float kFlapSlewRate     = 0.5f; // slew rate for normalized flaps setpoint [1/s]
static constexpr float kSpoilersSlewRate = 0.5f; // slew rate for normalized spoilers setpoint [1/s]

class ActuatorEffectivenessControlSurfaces : public ModuleParams, public ActuatorEffectiveness {
public:
    static constexpr int MAX_COUNT = 8;

    enum class Type : int32_t {
        // This matches with the parameter
        LeftAileron          = 1,
        RightAileron         = 2,
        Elevator             = 3,
        Rudder               = 4,
        LeftElevon           = 5,
        RightElevon          = 6,
        LeftVTail            = 7,
        RightVTail           = 8,
        LeftFlap             = 9,
        RightFlap            = 10,
        Airbrake             = 11,
        Custom               = 12,
        LeftATail            = 13,
        RightATail           = 14,
        SingleChannelAileron = 15,
        SteeringWheel        = 16,
        LeftSpoiler          = 17,
        RightSpoiler         = 18,
    };

    struct Params {
        Type type;

        matrix::Vector3f torque;
        float            trim;
        float            scale_flap;
        float            scale_spoiler;
    };

    ActuatorEffectivenessControlSurfaces(ModuleParams *parent);
    virtual ~ActuatorEffectivenessControlSurfaces() = default;

    bool addActuators(Configuration &configuration);

    const char *name() const override {
        return "Control Surfaces";
    }

    int count() const {
        return _count;
    }

    const Params &config(int idx) const {
        return _params[idx];
    }

    void applyFlaps(float flaps_control, int first_actuator_idx, float dt, ActuatorVector &actuator_sp);
    void applySpoilers(float spoilers_control, int first_actuator_idx, float dt, ActuatorVector &actuator_sp);

private:
    void updateParams() override;

    struct ParamHandles {
        param_t type;

        param_t torque[3];
        param_t trim;
        param_t scale_flap;
        param_t scale_spoiler;
    };
    ParamHandles _param_handles[MAX_COUNT];
    param_t      _count_handle;

    Params _params[MAX_COUNT]{};
    int    _count{0};

    SlewRate<float> _flaps_setpoint_with_slewrate;
    SlewRate<float> _spoilers_setpoint_with_slewrate;
};
