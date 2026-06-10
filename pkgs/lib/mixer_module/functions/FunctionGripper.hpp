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

#include <uORB/topics/gripper.h>

/**
 * @brief Function: Gripper (Used for actuating a Gripper)
 */
class FunctionGripper : public FunctionProviderBase
{
public:
	FunctionGripper() = default;
	static FunctionProviderBase *allocate(const Context &context) { return new FunctionGripper(); }

	void update() override
	{
		gripper_s gripper;

		if (_gripper_sub.update(&gripper)) {
			if (gripper.command == gripper_s::COMMAND_RELEASE) {
				_data = -1.f; // Minimum command for release

			} else if (gripper.command == gripper_s::COMMAND_GRAB) {
				_data = 1.f; // Maximum command for grab

			}
		}
	}

	float value(OutputFunction func) override { return _data; }

private:
	uORB::Subscription _gripper_sub{ORB_ID(gripper)};
	float _data{-1.f};
};
