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

#include <stdint.h>

#include <uORB/topics/sensor_optical_flow.h>

#include <com/hex/equipment/flow/Measurement.hpp>

class UavcanFlowBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanFlowBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void flow_sub_cb(const uavcan::ReceivedDataStructure<com::hex::equipment::flow::Measurement> &msg);

    typedef uavcan::MethodBinder<UavcanFlowBridge *,
                                 void (UavcanFlowBridge::*)(const uavcan::ReceivedDataStructure<com::hex::equipment::flow::Measurement> &)>
        FlowCbBinder;

    uavcan::Subscriber<com::hex::equipment::flow::Measurement, FlowCbBinder> _sub_flow;
};
