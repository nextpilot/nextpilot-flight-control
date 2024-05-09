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
 * @file ButtonPublisher.hpp
 *
 * Library for button functionality.
 *
 */

#pragma once

#include <drivers/drv_tone_alarm.h>
#include <hrtimer.h>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/button_event.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/led_control.h>
#include <uORB/topics/tune_control.h>

using namespace nextpilot;

class ButtonPublisher
{
public:
	ButtonPublisher();
	~ButtonPublisher() = default;

	/**
	 * Function for publishing safety button trigger event
	 */
	void safetyButtonTriggerEvent();

	/**
	 * Function for publishing pairing button trigger event
	 */
	void pairingButtonTriggerEvent();

	static constexpr uint8_t PAIRING_BUTTON_EVENT_COUNT{3};

private:
	uORB::Publication<button_event_s>		_safety_button_pub{ORB_ID(safety_button)};
	uORB::Publication<vehicle_command_s>	_vehicle_command_pub{ORB_ID(vehicle_command)};
	uORB::Publication<led_control_s> 		_led_control_pub{ORB_ID(led_control)};
	uORB::Publication<tune_control_s> 		_tune_control_pub{ORB_ID(tune_control)};
};
