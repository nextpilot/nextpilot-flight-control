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

#include <uavcan/equipment/air_data/StaticPressure.hpp>
#include <uavcan/equipment/air_data/StaticTemperature.hpp>

class UavcanBarometerBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanBarometerBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void air_pressure_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticPressure> &msg);
    void air_temperature_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticTemperature> &msg);

    int init_driver(uavcan_bridge::Channel *channel) override;

    typedef uavcan::MethodBinder<UavcanBarometerBridge *,
                                 void (UavcanBarometerBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticPressure> &)>
        AirPressureCbBinder;

    typedef uavcan::MethodBinder<UavcanBarometerBridge *,
                                 void (UavcanBarometerBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticTemperature> &)>
        AirTemperatureCbBinder;

    uavcan::Subscriber<uavcan::equipment::air_data::StaticPressure, AirPressureCbBinder>       _sub_air_pressure_data;
    uavcan::Subscriber<uavcan::equipment::air_data::StaticTemperature, AirTemperatureCbBinder> _sub_air_temperature_data;

    float _last_temperature_kelvin{NAN};
};
