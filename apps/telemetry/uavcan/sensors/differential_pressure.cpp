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

#include "differential_pressure.hpp"

#include <hrtimer.h>
#include <geo/geo.h>
#include <param/param.h>
#include <systemlib/err.h>

const char *const UavcanDifferentialPressureBridge::NAME = "differential_pressure";

UavcanDifferentialPressureBridge::UavcanDifferentialPressureBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_differential_pressure", ORB_ID(differential_pressure)),
    _sub_air(node) {
}

int UavcanDifferentialPressureBridge::init() {
    int res = _sub_air.start(AirCbBinder(this, &UavcanDifferentialPressureBridge::air_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanDifferentialPressureBridge::air_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::air_data::RawAirData> &msg) {
    const hrt_abstime timestamp_sample = hrt_absolute_time();

    _device_id.devid_s.devtype = DRV_DIFF_PRESS_DEVTYPE_UAVCAN;
    _device_id.devid_s.address = msg.getSrcNodeID().get() & 0xFF;

    float diff_press_pa = msg.differential_pressure;
    float temperature_c = msg.static_air_temperature + CONSTANTS_ABSOLUTE_NULL_CELSIUS;

    differential_pressure_s report{};
    report.timestamp_sample         = timestamp_sample;
    report.device_id                = _device_id.devid;
    report.differential_pressure_pa = diff_press_pa;
    report.temperature              = temperature_c;
    report.timestamp                = hrt_absolute_time();

    publish(msg.getSrcNodeID().get(), &report);
}
