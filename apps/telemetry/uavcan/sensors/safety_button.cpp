/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <hrtimer.h>
#include <parameters/param.h>
#include "safety_button.hpp"
#include <math.h>

const char *const UavcanSafetyButtonBridge::NAME = "safety_button";

using namespace time_literals;

UavcanSafetyButtonBridge::UavcanSafetyButtonBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_safety_button", ORB_ID(button_event)), _sub_button(node) {
}

int UavcanSafetyButtonBridge::init() {
    int res = _sub_button.start(ButtonCbBinder(this, &UavcanSafetyButtonBridge::button_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanSafetyButtonBridge::button_sub_cb(const uavcan::ReceivedDataStructure<ardupilot::indication::Button> &msg) {
    bool is_safety = msg.button == ardupilot::indication::Button::BUTTON_SAFETY;
    bool pressed   = msg.press_time >= 10; // 0.1s increments (1s press time for safety button trigger event)

    // Detect safety button trigger event
    if (is_safety && pressed) {
        _button_publisher.safetyButtonTriggerEvent();
    }

    // Detect pairing button trigger event
    if (is_safety) {
        if (hrt_elapsed_time(&_start_timestamp) > 2_s) {
            _start_timestamp        = hrt_absolute_time();
            _pairing_button_counter = 0u;
        }

        hrt_abstime press_time_us = (msg.press_time * 100 * 1000);
        hrt_abstime elasped_time  = hrt_elapsed_time(&_new_press_timestamp);

        if (elasped_time > press_time_us) {
            _pairing_button_counter++;
            _new_press_timestamp = hrt_absolute_time();
        }

        if (_pairing_button_counter == ButtonPublisher::PAIRING_BUTTON_EVENT_COUNT) {
            _button_publisher.pairingButtonTriggerEvent();
            _start_timestamp = 0u;
        }
    }
}

int UavcanSafetyButtonBridge::init_driver(uavcan_bridge::Channel *channel) {
    return PX4_OK;
}
