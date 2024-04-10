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

#include <uORB/topics/vehicle_command.h>

/**
 * Functions: Offboard_Actuator_Set1 ... Offboard_Actuator_Set6
 */
class FunctionActuatorSet : public FunctionProviderBase
{
public:
	FunctionActuatorSet()
	{
		for (int i = 0; i < max_num_actuators; ++i) {
			_data[i] = NAN;
		}
	}

	static FunctionProviderBase *allocate(const Context &context) { return new FunctionActuatorSet(); }

	void update() override
	{
		vehicle_command_s vehicle_command;

		while (_topic.update(&vehicle_command)) {
			if (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_SET_ACTUATOR) {
				int index = (int)(vehicle_command.param7 + 0.5f);

				if (index == 0) {
					if (PX4_ISFINITE(vehicle_command.param1)) { _data[0] = vehicle_command.param1; }

					if (PX4_ISFINITE(vehicle_command.param2)) {_data[1] = vehicle_command.param2; }

					if (PX4_ISFINITE(vehicle_command.param3)) {_data[2] = vehicle_command.param3; }

					if (PX4_ISFINITE(vehicle_command.param4)) {_data[3] = vehicle_command.param4; }

					if (PX4_ISFINITE(vehicle_command.param5)) {_data[4] = vehicle_command.param5; }

					if (PX4_ISFINITE(vehicle_command.param6)) {_data[5] = vehicle_command.param6; }
				}
			}
		}
	}

	float value(OutputFunction func) override { return _data[(int)func - (int)OutputFunction::Offboard_Actuator_Set1]; }

private:
	static constexpr int max_num_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	float _data[max_num_actuators] {};
};
