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

#include <uORB/topics/landing_gear.h>

/**
 * Functions: Landing_Gear
 */
class FunctionLandingGear : public FunctionProviderBase
{
public:
	FunctionLandingGear() = default;
	static FunctionProviderBase *allocate(const Context &context) { return new FunctionLandingGear(); }

	void update() override
	{
		landing_gear_s landing_gear;

		if (_topic.update(&landing_gear)) {
			if (landing_gear.landing_gear == landing_gear_s::GEAR_DOWN) {
				_data = -1.f;

			} else if (landing_gear.landing_gear == landing_gear_s::GEAR_UP) {
				_data = 1.f;
			}
		}
	}

	float value(OutputFunction func) override { return _data; }

private:
	uORB::Subscription _topic{ORB_ID(landing_gear)};
	float _data{-1.f};
};
