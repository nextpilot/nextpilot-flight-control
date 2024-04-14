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
 * @file ButtonPublisher.cpp
 *
 * Library for button functionality.
 *
 */

#include <button/ButtonPublisher.hpp>

using namespace time_literals;

ButtonPublisher::ButtonPublisher()
{
	_safety_button_pub.advertise();
}

void ButtonPublisher::safetyButtonTriggerEvent()
{


	button_event_s safety_button{};
	safety_button.triggered = true;
	safety_button.timestamp = hrt_absolute_time();

	_safety_button_pub.publish(safety_button);
}

void ButtonPublisher::pairingButtonTriggerEvent()
{
	vehicle_command_s vcmd{};
	vcmd.command = vehicle_command_s::VEHICLE_CMD_START_RX_PAIR;
	vcmd.param1 = 10.f; // GCS pairing request handled by a companion.
	vcmd.timestamp = hrt_absolute_time();
	_vehicle_command_pub.publish(vcmd);
	PX4_DEBUG("Sending GCS pairing request");

	led_control_s led_control{};
	led_control.led_mask = 0xff;
	led_control.mode = led_control_s::MODE_BLINK_FAST;
	led_control.color = led_control_s::COLOR_GREEN;
	led_control.num_blinks = 1;
	led_control.priority = 0;
	led_control.timestamp = hrt_absolute_time();
	_led_control_pub.publish(led_control);

	tune_control_s tune_control{};
	tune_control.tune_id = tune_control_s::TUNE_ID_NOTIFY_POSITIVE;
	tune_control.volume = tune_control_s::VOLUME_LEVEL_DEFAULT;
	tune_control.timestamp = hrt_absolute_time();
	_tune_control_pub.publish(tune_control);
}
