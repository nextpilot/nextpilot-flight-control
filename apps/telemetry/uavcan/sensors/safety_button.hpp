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

#include "sensor_bridge.hpp"
#include "button/ButtonPublisher.hpp"

#include <ardupilot/indication/Button.hpp>

class UavcanSafetyButtonBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanSafetyButtonBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    int init_driver(uavcan_bridge::Channel *channel) override;

    void button_sub_cb(const uavcan::ReceivedDataStructure<ardupilot::indication::Button> &msg);

    typedef uavcan::MethodBinder<UavcanSafetyButtonBridge *,
                                 void (UavcanSafetyButtonBridge::*)(const uavcan::ReceivedDataStructure<ardupilot::indication::Button> &)>
        ButtonCbBinder;

    uavcan::Subscriber<ardupilot::indication::Button, ButtonCbBinder> _sub_button;
    ButtonPublisher                                                   _button_publisher;
    uint8_t                                                           _pairing_button_counter{0u};
    hrt_abstime                                                       _start_timestamp{0};
    hrt_abstime                                                       _new_press_timestamp{0};
};
