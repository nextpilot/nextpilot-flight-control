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
 * @file FlightTaskTranstion.cpp
 */

#include "FlightTaskTransition.hpp"

using namespace matrix;

FlightTaskTransition::FlightTaskTransition() {
    _param_handle_pitch_cruise_degrees = param_find("FW_PSP_OFF");

    if (_param_handle_pitch_cruise_degrees != PARAM_INVALID) {
        param_get(_param_handle_pitch_cruise_degrees, &_param_pitch_cruise_degrees);
    }
}

bool FlightTaskTransition::updateInitialize() {
    updateParameters();
    return FlightTask::updateInitialize();
}

void FlightTaskTransition::updateParameters() {
    // check for parameter updates
    if (_parameter_update_sub.updated()) {
        // clear update
        parameter_update_s pupdate;
        _parameter_update_sub.copy(&pupdate);

        // update parameters from storage
        if (_param_handle_pitch_cruise_degrees != PARAM_INVALID) {
            param_get(_param_handle_pitch_cruise_degrees, &_param_pitch_cruise_degrees);
        }
    }
}

bool FlightTaskTransition::activate(const trajectory_setpoint_s &last_setpoint) {
    bool ret = FlightTask::activate(last_setpoint);

    _vel_z_filter.setParameters(math::constrain(_deltatime, 0.01f, 0.1f), _vel_z_filter_time_const);

    if (PX4_ISFINITE(last_setpoint.velocity[2])) {
        _vel_z_filter.reset(last_setpoint.velocity[2]);

    } else {
        _vel_z_filter.reset(_velocity(2));
    }

    _velocity_setpoint(2) = _vel_z_filter.getState();

    _sub_vehicle_status.update();

    const bool is_vtol_front_transition = _sub_vehicle_status.get().in_transition_mode && _sub_vehicle_status.get().in_transition_to_fw;

    if (is_vtol_front_transition) {
        _gear.landing_gear = landing_gear_s::GEAR_UP;

    } else {
        _gear.landing_gear = landing_gear_s::GEAR_DOWN;
    }

    return ret;
}

bool FlightTaskTransition::update() {
    // tailsitters will override attitude and thrust setpoint
    // tiltrotors and standard vtol will overrride roll and pitch setpoint but keep vertical thrust setpoint
    bool ret = FlightTask::update();

    _position_setpoint.setAll(NAN);

    // calculate a horizontal acceleration vector which corresponds to an attitude composed of pitch up by _param_pitch_cruise_degrees
    // and zero roll angle
    const Vector2f tmp          = Dcm2f(_yaw) * Vector2f(-1.0f, 0.0f);
    _acceleration_setpoint.xy() = tmp * tanf(math::radians(_param_pitch_cruise_degrees)) * CONSTANTS_ONE_G;

    // slowly move vertical velocity setpoint to zero
    _vel_z_filter.setParameters(math::constrain(_deltatime, 0.01f, 0.1f), _vel_z_filter_time_const);
    _velocity_setpoint(2) = _vel_z_filter.update(0.0f);

    _yaw_setpoint = NAN;
    return ret;
}
