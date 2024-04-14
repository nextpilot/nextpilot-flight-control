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

#include <uavcan/equipment/ahrs/RawIMU.hpp>

class UavcanAccelBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanAccelBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void imu_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::RawIMU> &msg);

    int init_driver(uavcan_bridge::Channel *channel) override;

    typedef uavcan::MethodBinder<UavcanAccelBridge *,
                                 void (UavcanAccelBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::RawIMU> &)>
        ImuCbBinder;

    uavcan::Subscriber<uavcan::equipment::ahrs::RawIMU, ImuCbBinder> _sub_imu_data;
};
