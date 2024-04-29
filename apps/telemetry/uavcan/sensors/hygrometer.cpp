/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "hygrometer.hpp"

#include <hrtimer.h>
#include <geo/geo.h>

const char *const UavcanHygrometerBridge::NAME = "hygrometer_sensor";

UavcanHygrometerBridge::UavcanHygrometerBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_hygrometer_sensor", ORB_ID(sensor_hygrometer)),
    _sub_hygro(node) {
}

int UavcanHygrometerBridge::init() {
    int res = _sub_hygro.start(HygroCbBinder(this, &UavcanHygrometerBridge::hygro_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanHygrometerBridge::hygro_sub_cb(const uavcan::ReceivedDataStructure<dronecan::sensors::hygrometer::Hygrometer>
                                              &msg) {
    const hrt_abstime timestamp_sample = hrt_absolute_time();

    sensor_hygrometer_s report{};
    report.timestamp_sample = timestamp_sample;
    report.device_id        = 0; // TODO
    report.temperature      = msg.temperature + CONSTANTS_ABSOLUTE_NULL_CELSIUS;
    report.humidity         = msg.humidity;
    report.timestamp        = hrt_absolute_time();

    publish(msg.getSrcNodeID().get(), &report);
}
