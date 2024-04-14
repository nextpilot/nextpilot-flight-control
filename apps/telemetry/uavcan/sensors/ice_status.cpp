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

#include "ice_status.hpp"
#include <uORB/topics/internal_combustion_engine_status.h>

const char *const UavcanIceStatusBridge::NAME = "ice_status";

UavcanIceStatusBridge::UavcanIceStatusBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_ice_status", ORB_ID(internal_combustion_engine_status)),
    _sub_ice_status_data(node) {
}

int UavcanIceStatusBridge::init() {
    int res = _sub_ice_status_data.start(IceStatusCbBinder(this, &UavcanIceStatusBridge::ice_status_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanIceStatusBridge::ice_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ice::reciprocating::Status> &msg) {
    auto report                               = ::internal_combustion_engine_status_s();
    report.timestamp                          = hrt_absolute_time();
    report.state                              = msg.state;
    report.flags                              = msg.flags;
    report.engine_load_percent                = msg.engine_load_percent;
    report.engine_speed_rpm                   = msg.engine_speed_rpm;
    report.spark_dwell_time_ms                = msg.spark_dwell_time_ms;
    report.atmospheric_pressure_kpa           = msg.atmospheric_pressure_kpa;
    report.intake_manifold_pressure_kpa       = msg.intake_manifold_pressure_kpa;
    report.intake_manifold_temperature        = msg.intake_manifold_temperature;
    report.coolant_temperature                = msg.coolant_temperature;
    report.oil_pressure                       = msg.oil_pressure;
    report.oil_temperature                    = msg.oil_temperature;
    report.fuel_pressure                      = msg.fuel_pressure;
    report.fuel_consumption_rate_cm3pm        = msg.fuel_consumption_rate_cm3pm;
    report.estimated_consumed_fuel_volume_cm3 = msg.estimated_consumed_fuel_volume_cm3;
    report.throttle_position_percent          = msg.throttle_position_percent;
    report.ecu_index                          = msg.ecu_index;
    report.spark_plug_usage                   = msg.spark_plug_usage;

    if (msg.cylinder_status.size() > 0) {
        report.ignition_timing_deg       = msg.cylinder_status[0].ignition_timing_deg;
        report.injection_time_ms         = msg.cylinder_status[0].injection_time_ms;
        report.cylinder_head_temperature = msg.cylinder_status[0].cylinder_head_temperature;
        report.exhaust_gas_temperature   = msg.cylinder_status[0].exhaust_gas_temperature;
        report.lambda_coefficient        = msg.cylinder_status[0].lambda_coefficient;
    }

    publish(msg.getSrcNodeID().get(), &report);
}

int UavcanIceStatusBridge::init_driver(uavcan_bridge::Channel *channel) {
    return PX4_OK;
}
