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

#include <uORB/topics/landing_gear_wheel.h>

/**
 * Functions: Landing_Gear_Wheel
 */
class FunctionLandingGearWheel : public FunctionProviderBase
{
public:
	FunctionLandingGearWheel() = default;
	static FunctionProviderBase *allocate(const Context &context) { return new FunctionLandingGearWheel(); }

	void update() override
	{
		landing_gear_wheel_s landing_gear_wheel;

		if (_topic.update(&landing_gear_wheel)) {
			_data = landing_gear_wheel.normalized_wheel_setpoint;
		}
	}

	float value(OutputFunction func) override { return _data; }

private:
	uORB::Subscription _topic{ORB_ID(landing_gear_wheel)};
	float _data{0.f};
};
