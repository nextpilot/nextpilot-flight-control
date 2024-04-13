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
 * Functions: Motor1 ... MotorMax
 */
class FunctionMotors : public FunctionProviderBase
{
public:
	static_assert(actuator_motors_s::NUM_CONTROLS == (int)OutputFunction::MotorMax - (int)OutputFunction::Motor1 + 1,
		      "Unexpected num motors");

	static_assert(actuator_motors_s::ACTUATOR_FUNCTION_MOTOR1 == (int)OutputFunction::Motor1, "Unexpected motor idx");

	FunctionMotors(const Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_motors)),
		_thrust_factor(context.thrust_factor)
	{
		for (int i = 0; i < actuator_motors_s::NUM_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	static FunctionProviderBase *allocate(const Context &context) { return new FunctionMotors(context); }

	void update() override
	{
		if (_topic.update(&_data)) {
			updateValues(_data.reversible_flags, _thrust_factor, _data.control, actuator_motors_s::NUM_CONTROLS);
		}
	}

	float value(OutputFunction func) override { return _data.control[(int)func - (int)OutputFunction::Motor1]; }

	bool allowPrearmControl() const override { return false; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { return &_topic; }

	bool getLatestSampleTimestamp(hrt_abstime &t) const override { t = _data.timestamp_sample; return t != 0; }

	static inline void updateValues(uint32_t reversible, float thrust_factor, float *values, int num_values)
	{
		if (thrust_factor > 0.f && thrust_factor <= 1.f) {
			// thrust factor
			//  rel_thrust = factor * x^2 + (1-factor) * x,
			const float a = thrust_factor;
			const float b = (1.f - thrust_factor);

			// don't recompute for all values (ax^2+bx+c=0)
			const float tmp1 = b / (2.f * a);
			const float tmp2 = b * b / (4.f * a * a);

			for (int i = 0; i < num_values; ++i) {
				float control = values[i];

				if (control > 0.f) {
					values[i] = -tmp1 + sqrtf(tmp2 + (control / a));

				} else if (control < -0.f) {
					values[i] =  tmp1 - sqrtf(tmp2 - (control / a));

				} else {
					values[i] = 0.f;
				}
			}
		}

		for (int i = 0; i < num_values; ++i) {
			if ((reversible & (1u << i)) == 0) {
				if (values[i] < -FLT_EPSILON) {
					values[i] = NAN;

				} else {
					// remap from [0, 1] to [-1, 1]
					values[i] = values[i] * 2.f - 1.f;
				}
			}
		}
	}

	bool reversible(OutputFunction func) const override { return _data.reversible_flags & (1u << ((int)func - (int)OutputFunction::Motor1)); }

private:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_motors_s _data{};
	const float &_thrust_factor;
};
