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
 * @author Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#pragma once

#include "sensor_bridge.hpp"

#include <uavcan/equipment/ice/reciprocating/Status.hpp>

class UavcanIceStatusBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanIceStatusBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void ice_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ice::reciprocating::Status> &msg);
    int  init_driver(uavcan_bridge::Channel *channel) override;

    typedef uavcan::MethodBinder<UavcanIceStatusBridge *,
                                 void (UavcanIceStatusBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::ice::reciprocating::Status> &)>
        IceStatusCbBinder;

    uavcan::Subscriber<uavcan::equipment::ice::reciprocating::Status, IceStatusCbBinder> _sub_ice_status_data;
};
