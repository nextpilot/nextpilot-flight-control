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
 * @file WeatherVane.hpp
 * @author Ivo Drescher
 * @author Roman Bapst <roman@auterion.com>
 *
 * Weathervane controller.
 *
 */

#pragma once

#include <module_params.hpp>
#include <matrix/matrix/math.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_status.h>

class WeatherVane : public ModuleParams
{
public:
	WeatherVane(ModuleParams *parent);

	~WeatherVane() = default;

	void setNavigatorForceDisabled(bool navigator_force_disabled) { _navigator_force_disabled = navigator_force_disabled; };

	bool isActive() {return _is_active;}

	void update();

	float getWeathervaneYawrate();

private:
	uORB::Subscription _vehicle_attitude_setpoint_sub{ORB_ID(vehicle_attitude_setpoint)};
	uORB::Subscription _vehicle_local_position_sub{ORB_ID(vehicle_local_position)};
	uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

	bool _is_active{false};

	// local copies of status such that we don't need to copy uORB messages all the time
	bool _flag_control_manual_enabled{false};
	bool _flag_control_position_enabled{false};
	bool _navigator_force_disabled{false};

	DEFINE_PARAMETERS(
		(ParamBool<params_id::WV_EN>) _param_wv_en,
		(ParamFloat<params_id::WV_ROLL_MIN>) _param_wv_roll_min,
		(ParamFloat<params_id::WV_GAIN>) _param_wv_gain,
		(ParamFloat<params_id::WV_YRATE_MAX>) _param_wv_yrate_max
	)

};
