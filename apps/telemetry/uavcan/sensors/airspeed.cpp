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

#include <drivers/drv_hrt.h>
#include "airspeed.hpp"
#include <math.h>
#include <lib/geo/geo.h> // For CONSTANTS_*

const char *const UavcanAirspeedBridge::NAME = "airspeed";

UavcanAirspeedBridge::UavcanAirspeedBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_airspeed", ORB_ID(airspeed)),
    _sub_ias_data(node),
    _sub_tas_data(node),
    _sub_oat_data(node) {
}

int UavcanAirspeedBridge::init() {
    int res = _sub_ias_data.start(IASCbBinder(this, &UavcanAirspeedBridge::ias_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    int res2 = _sub_tas_data.start(TASCbBinder(this, &UavcanAirspeedBridge::tas_sub_cb));

    if (res2 < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res2);
        return res2;
    }

    int res3 = _sub_oat_data.start(OATCbBinder(this, &UavcanAirspeedBridge::oat_sub_cb));

    if (res3 < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res3);
        return res3;
    }

    return 0;
}

void UavcanAirspeedBridge::oat_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::StaticTemperature> &msg) {
    _last_outside_air_temp_k = msg.static_temperature;
}

void UavcanAirspeedBridge::tas_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::TrueAirspeed> &msg) {
    _last_tas_m_s = msg.true_airspeed;
}
void UavcanAirspeedBridge::ias_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::IndicatedAirspeed> &msg) {
    airspeed_s report{};

    /*
     * FIXME HACK
     * This code used to rely on msg.getMonotonicTimestamp().toUSec() instead of HRT.
     * It stopped working when the time sync feature has been introduced, because it caused libuavcan
     * to use an independent time source (based on hardware TIM5) instead of HRT.
     * The proper solution is to be developed.
     */
    report.timestamp               = hrt_absolute_time();
    report.indicated_airspeed_m_s  = msg.indicated_airspeed;
    report.true_airspeed_m_s       = _last_tas_m_s;
    report.air_temperature_celsius = _last_outside_air_temp_k + CONSTANTS_ABSOLUTE_NULL_CELSIUS;

    publish(msg.getSrcNodeID().get(), &report);
}
