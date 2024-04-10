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
 * @file vtol_takeoff.h
 *
 * Helper class to do a VTOL takeoff and transition into a loiter.
 *
 */

#pragma once

#include "navigator_mode.h"
#include "mission_block.h"
#include <mathlib/mathlib.h>
#include <module/module_params.hpp>

using namespace nextpilot::global_params;

class VtolTakeoff : public MissionBlock, public ModuleParams {
public:
    VtolTakeoff(Navigator *navigator);
    ~VtolTakeoff() = default;

    void on_activation() override;
    void on_active() override;

    void setTransitionAltitudeAbsolute(const float alt_amsl) {
        _transition_alt_amsl = alt_amsl;
    }

    void setLoiterLocation(matrix::Vector2d loiter_location) {
        _loiter_location = loiter_location;
    }
    void setLoiterHeight(const float height_m) {
        _loiter_height = height_m;
    }

private:
    enum class vtol_takeoff_state {
        TAKEOFF_HOVER = 0,
        ALIGN_HEADING,
        TRANSITION,
        CLIMB,
        ABORT_TAKEOFF_AND_LAND
    } _takeoff_state;

    float            _transition_alt_amsl{0.f}; // absolute altitude at which vehicle will transition to forward flight
    matrix::Vector2d _loiter_location;
    float            _loiter_height{0};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::VTO_LOITER_ALT>)_param_loiter_alt)

    void set_takeoff_position();
};
