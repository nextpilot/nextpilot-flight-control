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
#include <uORB/topics/airspeed.h>

#include <uavcan/equipment/air_data/IndicatedAirspeed.hpp>
#include <uavcan/equipment/air_data/TrueAirspeed.hpp>
#include <uavcan/equipment/air_data/StaticTemperature.hpp>

class UavcanAirspeedBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanAirspeedBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    void ias_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::IndicatedAirspeed> &msg);
    void tas_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::TrueAirspeed> &msg);
    void oat_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticTemperature> &msg);

    typedef uavcan::MethodBinder<UavcanAirspeedBridge *,
                                 void (UavcanAirspeedBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::IndicatedAirspeed> &)>
        IASCbBinder;

    typedef uavcan::MethodBinder<UavcanAirspeedBridge *,
                                 void (UavcanAirspeedBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::TrueAirspeed> &)>
        TASCbBinder;

    typedef uavcan::MethodBinder<UavcanAirspeedBridge *,
                                 void (UavcanAirspeedBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticTemperature> &)>
        OATCbBinder;

    uavcan::Subscriber<uavcan::equipment::air_data::IndicatedAirspeed, IASCbBinder> _sub_ias_data;
    uavcan::Subscriber<uavcan::equipment::air_data::TrueAirspeed, TASCbBinder>      _sub_tas_data;
    uavcan::Subscriber<uavcan::equipment::air_data::StaticTemperature, OATCbBinder> _sub_oat_data;

    float _last_tas_m_s{0.0f};
    float _last_outside_air_temp_k{0.0f};
};
