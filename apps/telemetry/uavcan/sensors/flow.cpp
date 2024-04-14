/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "flow.hpp"

#include <drivers/drv_hrt.h>

const char *const UavcanFlowBridge::NAME = "flow";

UavcanFlowBridge::UavcanFlowBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_flow", ORB_ID(sensor_optical_flow)),
    _sub_flow(node) {
}

int UavcanFlowBridge::init() {
    int res = _sub_flow.start(FlowCbBinder(this, &UavcanFlowBridge::flow_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanFlowBridge::flow_sub_cb(const uavcan::ReceivedDataStructure<com::hex::equipment::flow::Measurement> &msg) {
    sensor_optical_flow_s flow{};
    flow.timestamp_sample = hrt_absolute_time(); // TODO

    device::Device::DeviceId device_id;
    device_id.devid_s.bus_type = device::Device::DeviceBusType::DeviceBusType_UAVCAN;
    device_id.devid_s.bus      = 0;
    device_id.devid_s.devtype  = DRV_FLOW_DEVTYPE_UAVCAN;
    device_id.devid_s.address  = msg.getSrcNodeID().get() & 0xFF;

    flow.device_id = device_id.devid;

    flow.pixel_flow[0] = msg.flow_integral[0];
    flow.pixel_flow[1] = msg.flow_integral[1];

    flow.integration_timespan_us = 1.e6f * msg.integration_interval; // s -> us

    flow.quality = msg.quality;

    if (PX4_ISFINITE(msg.rate_gyro_integral[0]) && PX4_ISFINITE(msg.rate_gyro_integral[1])) {
        flow.delta_angle[0]        = msg.rate_gyro_integral[0];
        flow.delta_angle[1]        = msg.rate_gyro_integral[1];
        flow.delta_angle[2]        = 0.f;
        flow.delta_angle_available = true;

    } else {
        flow.delta_angle[0] = NAN;
        flow.delta_angle[1] = NAN;
        flow.delta_angle[2] = NAN;
    }

    flow.max_flow_rate       = NAN;
    flow.min_ground_distance = NAN;
    flow.max_ground_distance = NAN;

    flow.timestamp = hrt_absolute_time();

    publish(msg.getSrcNodeID().get(), &flow);
}
