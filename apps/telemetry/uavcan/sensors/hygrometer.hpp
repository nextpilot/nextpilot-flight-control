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

#include <uORB/topics/sensor_hygrometer.h>
#include "sensor_bridge.hpp"

#include <dronecan/sensors/hygrometer/Hygrometer.hpp>

class UavcanHygrometerBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanHygrometerBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void hygro_sub_cb(const uavcan::ReceivedDataStructure<dronecan::sensors::hygrometer::Hygrometer> &msg);

    typedef uavcan::MethodBinder<UavcanHygrometerBridge *,
                                 void (UavcanHygrometerBridge::*)(const uavcan::ReceivedDataStructure<dronecan::sensors::hygrometer::Hygrometer> &)>
        HygroCbBinder;

    uavcan::Subscriber<dronecan::sensors::hygrometer::Hygrometer, HygroCbBinder> _sub_hygro;
};
