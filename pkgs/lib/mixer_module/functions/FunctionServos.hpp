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

#include <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
class FunctionServos : public FunctionProviderBase
{
public:
	static_assert(actuator_servos_s::NUM_CONTROLS == (int)OutputFunction::ServoMax - (int)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(const Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		for (int i = 0; i < actuator_servos_s::NUM_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	static FunctionProviderBase *allocate(const Context &context) { return new FunctionServos(context); }

	void update() override { _topic.update(&_data); }
	float value(OutputFunction func) override { return _data.control[(int)func - (int)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { return &_topic; }

	float defaultFailsafeValue(OutputFunction func) const override { return 0.f; }

private:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};
