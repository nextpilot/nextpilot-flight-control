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
 * @author RJ Gritter <rjgritter657@gmail.com>
 */

#pragma once

#include "sensor_bridge.hpp"
#include <uORB/topics/distance_sensor.h>
#include <drivers/rangefinder/PX4Rangefinder.hpp>

#include <uavcan/equipment/range_sensor/Measurement.hpp>

class UavcanRangefinderBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanRangefinderBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    int init_driver(uavcan_bridge::Channel *channel) override;

    void range_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::range_sensor::Measurement> &msg);

    typedef uavcan::MethodBinder<UavcanRangefinderBridge *,
                                 void (UavcanRangefinderBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::range_sensor::Measurement> &)>
        RangeCbBinder;

    uavcan::Subscriber<uavcan::equipment::range_sensor::Measurement, RangeCbBinder> _sub_range_data;

    float _range_min_m{0.0f};
    float _range_max_m{0.0f};

    bool _inited{false};
};
