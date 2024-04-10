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

#include "FunctionProviderBase.hpp"

#include <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
class FunctionManualRC : public FunctionProviderBase
{
public:
	FunctionManualRC()
	{
		for (int i = 0; i < num_data_points; ++i) {
			_data[i] = NAN;
		}
	}

	static FunctionProviderBase *allocate(const Context &context) { return new FunctionManualRC(); }

	void update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		if (_topic.update(&manual_control_setpoint)) {
			_data[0] = manual_control_setpoint.roll;
			_data[1] = manual_control_setpoint.pitch;
			_data[2] = manual_control_setpoint.throttle;
			_data[3] = manual_control_setpoint.yaw;
			_data[4] = manual_control_setpoint.flaps;
			_data[5] = manual_control_setpoint.aux1;
			_data[6] = manual_control_setpoint.aux2;
			_data[7] = manual_control_setpoint.aux3;
			_data[8] = manual_control_setpoint.aux4;
			_data[9] = manual_control_setpoint.aux5;
			_data[10] = manual_control_setpoint.aux6;
		}
	}

	float value(OutputFunction func) override { return _data[(int)func - (int)OutputFunction::RC_Roll]; }

private:
	static constexpr int num_data_points = 11;

	static_assert(num_data_points == (int)OutputFunction::RC_AUXMax - (int)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	float _data[num_data_points] {};
};
