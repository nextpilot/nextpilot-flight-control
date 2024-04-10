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

#include <uORB/topics/gimbal_controls.h>

/**
 * Functions: Gimbal_Roll .. Gimbal_Yaw
 */
class FunctionGimbal : public FunctionProviderBase
{
public:
	FunctionGimbal() = default;
	static FunctionProviderBase *allocate(const Context &context) { return new FunctionGimbal(); }

	void update() override
	{
		gimbal_controls_s gimbal_controls;

		if (_topic.update(&gimbal_controls)) {
			_data[0] = gimbal_controls.control[gimbal_controls_s::INDEX_ROLL];
			_data[1] = gimbal_controls.control[gimbal_controls_s::INDEX_PITCH];
			_data[2] = gimbal_controls.control[gimbal_controls_s::INDEX_YAW];
		}
	}

	float value(OutputFunction func) override { return _data[(int)func - (int)OutputFunction::Gimbal_Roll]; }

private:
	uORB::Subscription _topic{ORB_ID(gimbal_controls)};
	float _data[3] { NAN, NAN, NAN };
};
