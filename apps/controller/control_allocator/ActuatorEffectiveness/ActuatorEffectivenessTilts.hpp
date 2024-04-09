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

#include <px4_platform_common/module_params.hpp>

class ActuatorEffectivenessTilts : public ModuleParams, public ActuatorEffectiveness {
public:
    static constexpr int MAX_COUNT = 4;

    enum class Control : int32_t {
        // This matches with the parameter
        None        = 0,
        Yaw         = 1,
        Pitch       = 2,
        YawAndPitch = 3,
    };
    enum class TiltDirection : int32_t {
        // This matches with the parameter
        TowardsFront = 0,
        TowardsRight = 90,
    };

    struct Params {
        Control       control;
        float         min_angle;
        float         max_angle;
        TiltDirection tilt_direction;
    };

    ActuatorEffectivenessTilts(ModuleParams *parent);
    virtual ~ActuatorEffectivenessTilts() = default;

    bool addActuators(Configuration &configuration);

    const char *name() const override {
        return "Tilts";
    }

    int count() const {
        return _count;
    }

    const Params &config(int idx) const {
        return _params[idx];
    }

    void updateTorqueSign(const ActuatorEffectivenessRotors::Geometry &geometry, bool disable_pitch = false);

    bool hasYawControl() const;

private:
    void updateParams() override;

    struct ParamHandles {
        param_t control;
        param_t min_angle;
        param_t max_angle;
        param_t tilt_direction;
    };

    ParamHandles _param_handles[MAX_COUNT];
    param_t      _count_handle;

    Params _params[MAX_COUNT]{};
    int    _count{0};

    matrix::Vector3f _torque[MAX_COUNT]{};
};
