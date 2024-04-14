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
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include "sensor_bridge.hpp"

#include <uavcan/equipment/ahrs/MagneticFieldStrength.hpp>
#include <uavcan/equipment/ahrs/MagneticFieldStrength2.hpp>

class UavcanMagnetometerBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanMagnetometerBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    int init_driver(uavcan_bridge::Channel *channel) override;

    void mag_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::MagneticFieldStrength> &msg);
    void mag2_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::MagneticFieldStrength2> &msg);

    typedef uavcan::MethodBinder<UavcanMagnetometerBridge *,
                                 void (UavcanMagnetometerBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::MagneticFieldStrength> &)>
        MagCbBinder;

    typedef uavcan::MethodBinder<UavcanMagnetometerBridge *,
                                 void (UavcanMagnetometerBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::MagneticFieldStrength2> &)>
        Mag2CbBinder;

    uavcan::Subscriber<uavcan::equipment::ahrs::MagneticFieldStrength, MagCbBinder>   _sub_mag;
    uavcan::Subscriber<uavcan::equipment::ahrs::MagneticFieldStrength2, Mag2CbBinder> _sub_mag2;
};
