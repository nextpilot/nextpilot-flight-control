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
 * @author Jacob Crabill <jacob@flyvoly.com>
 */

#pragma once

#include <uORB/topics/differential_pressure.h>

#include "sensor_bridge.hpp"

#include <uavcan/equipment/air_data/RawAirData.hpp>

class UavcanDifferentialPressureBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanDifferentialPressureBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void air_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::RawAirData> &msg);

    typedef uavcan::MethodBinder<UavcanDifferentialPressureBridge *,
                                 void (UavcanDifferentialPressureBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::RawAirData> &)>
        AirCbBinder;

    uavcan::Subscriber<uavcan::equipment::air_data::RawAirData, AirCbBinder> _sub_air;
};
