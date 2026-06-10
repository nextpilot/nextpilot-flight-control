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
 * @file WeatherVane.cpp
 * Weathervane controller.
 *
 */

#include "WeatherVane.hpp"
#include <mathlib/mathlib.h>


WeatherVane::WeatherVane(ModuleParams *parent) :
	ModuleParams(parent)
{ }

void WeatherVane::update()
{
	vehicle_control_mode_s vehicle_control_mode;

	if (_vehicle_control_mode_sub.update(&vehicle_control_mode)) {
		_flag_control_manual_enabled = vehicle_control_mode.flag_control_manual_enabled;
		_flag_control_position_enabled = vehicle_control_mode.flag_control_position_enabled;
	}

	// Weathervane needs to be enabled by parameter
	// in manual we use weathervane just if position is controlled as well
	// in mission we use weathervane except for when navigator disables it
	_is_active = _param_wv_en.get()
		     && ((_flag_control_manual_enabled && _flag_control_position_enabled)
			 || (!_flag_control_manual_enabled && !_navigator_force_disabled));
}

float WeatherVane::getWeathervaneYawrate()
{
	// direction of desired body z axis represented in earth frame
	vehicle_attitude_setpoint_s vehicle_attitude_setpoint;
	_vehicle_attitude_setpoint_sub.copy(&vehicle_attitude_setpoint);
	matrix::Vector3f body_z_sp(matrix::Quatf(vehicle_attitude_setpoint.q_d).dcm_z()); // attitude setpoint body z axis

	// rotate desired body z axis into new frame which is rotated in z by the current
	// heading of the vehicle. we refer to this as the heading frame.
	vehicle_local_position_s vehicle_local_position{};
	_vehicle_local_position_sub.copy(&vehicle_local_position);
	matrix::Dcmf R_yaw = matrix::Eulerf(0.0f, 0.0f, -vehicle_local_position.heading);
	body_z_sp = R_yaw * body_z_sp;
	body_z_sp.normalize();

	float roll_sp = -asinf(body_z_sp(1));

	float roll_exceeding_treshold = 0.0f;
	float min_roll_rad = math::radians(_param_wv_roll_min.get());

	if (roll_sp > min_roll_rad) {
		roll_exceeding_treshold = roll_sp - min_roll_rad;

	} else if (roll_sp < -min_roll_rad) {
		roll_exceeding_treshold = roll_sp + min_roll_rad;
	}

	return math::constrain(roll_exceeding_treshold * _param_wv_gain.get(), -math::radians(_param_wv_yrate_max.get()),
			       math::radians(_param_wv_yrate_max.get()));
}
