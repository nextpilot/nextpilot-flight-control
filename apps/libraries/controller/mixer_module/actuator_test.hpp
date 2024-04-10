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

#include <mixer_module/output_functions.hpp>

#include <drivers/drv_pwm_output.h>
#include <uORB/topics/actuator_test.h>
#include <uORB/topics/actuator_motors.h>
#include <uORB/topics/actuator_servos_trim.h>
#include <uORB/Subscription.hpp>

static_assert(actuator_test_s::FUNCTION_MOTOR1 == (int)OutputFunction::Motor1, "define mismatch");
static_assert(actuator_test_s::MAX_NUM_MOTORS == (int)OutputFunction::MotorMax - (int)OutputFunction::Motor1 + 1,
	      "count mismatch");
static_assert(actuator_test_s::FUNCTION_SERVO1 == (int)OutputFunction::Servo1, "define mismatch");
static_assert(actuator_test_s::MAX_NUM_SERVOS == (int)OutputFunction::ServoMax - (int)OutputFunction::Servo1 + 1,
	      "count mismatch");

class ActuatorTest
{
public:
	static constexpr int MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	ActuatorTest(const OutputFunction function_assignments[MAX_ACTUATORS]);

	void reset();

	void update(int num_outputs, float thrust_curve);

	void overrideValues(float outputs[MAX_ACTUATORS], int num_outputs);

	bool inTestMode() const { return _in_test_mode; }

private:

	uORB::Subscription _actuator_test_sub{ORB_ID(actuator_test)};
	uORB::Subscription _actuator_motors_sub{ORB_ID(actuator_motors)};
	uORB::Subscription _actuator_servos_trim_sub{ORB_ID(actuator_servos_trim)};
	bool _in_test_mode{false};
	hrt_abstime _next_timeout{0};

	float _current_outputs[MAX_ACTUATORS];
	bool _output_overridden[MAX_ACTUATORS];
	const OutputFunction *_function_assignments;
};
