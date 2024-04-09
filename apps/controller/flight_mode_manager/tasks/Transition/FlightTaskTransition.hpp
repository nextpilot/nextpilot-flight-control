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
 * @file FlightTaskTransition.hpp
 *
 * Flight task for automatic VTOL transitions between hover and forward flight and vice versa.
 */

#pragma once

#include "FlightTask.hpp"
#include <lib/mathlib/math/filter/AlphaFilter.hpp>
#include <uORB/SubscriptionInterval.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_status.h>
#include <drivers/drv_hrt.h>

using namespace time_literals;

class FlightTaskTransition : public FlightTask {
public:
    FlightTaskTransition();

    virtual ~FlightTaskTransition() = default;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    bool updateInitialize() override;
    bool update() override;

private:
    static constexpr float _vel_z_filter_time_const = 2.0f;

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::SubscriptionData<vehicle_status_s> _sub_vehicle_status{ORB_ID(vehicle_status)};

    param_t _param_handle_pitch_cruise_degrees{PARAM_INVALID};
    float   _param_pitch_cruise_degrees{0.f};

    AlphaFilter<float> _vel_z_filter;

    void updateParameters();
};
