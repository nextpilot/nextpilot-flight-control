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
 * @file ghst_telemetry.cpp
 *
 * IRC Ghost (Immersion RC Ghost) telemetry.
 *
 * @author Igor Misic <igy1000mb@gmail.com>
 * @author Juraj Ciberlin <jciberlin1@gmail.com>
 */

#include "ghst_telemetry.hpp"
#include <lib/rc/ghst.hpp>

using time_literals::operator""_s;

GHSTTelemetry::GHSTTelemetry(int uart_fd) :
    _uart_fd(uart_fd) {
}

bool GHSTTelemetry::update(const hrt_abstime &now) {
    bool success = false;

    if ((now - _last_update) > (1_s / (UPDATE_RATE_HZ * NUM_DATA_TYPES))) {
        switch (_next_type) {
        case 0U:
            success = send_battery_status();
            break;

        case 1U:
            success = send_gps1_status();
            break;

        case 2U:
            success = send_gps2_status();
            break;

        default:
            success = false;
            break;
        }

        _last_update = now;
        _next_type   = (_next_type + 1U) % NUM_DATA_TYPES;
    }

    return success;
}

bool GHSTTelemetry::send_battery_status() {
    bool             success = false;
    float            voltage_in_10mV;
    float            current_in_10mA;
    float            fuel_in_10mAh;
    battery_status_s battery_status;

    if (_battery_status_sub.update(&battery_status)) {
        voltage_in_10mV = battery_status.voltage_filtered_v * FACTOR_VOLTS_TO_10MV;
        current_in_10mA = battery_status.current_filtered_a * FACTOR_AMPS_TO_10MA;
        fuel_in_10mAh   = battery_status.discharged_mah * FACTOR_MAH_TO_10MAH;
        success         = ghst_send_telemetry_battery_status(_uart_fd,
                                                             static_cast<uint16_t>(voltage_in_10mV),
                                                             static_cast<uint16_t>(current_in_10mA),
                                                             static_cast<uint16_t>(fuel_in_10mAh));
    }

    return success;
}

bool GHSTTelemetry::send_gps1_status() {
    sensor_gps_s vehicle_gps_position;

    if (!_vehicle_gps_position_sub.update(&vehicle_gps_position)) {
        return false;
    }

    int32_t  latitude  = vehicle_gps_position.lat;        // 1e-7 degrees
    int32_t  longitude = vehicle_gps_position.lon;        // 1e-7 degrees
    uint16_t altitude  = vehicle_gps_position.alt / 1000; // mm -> m

    return ghst_send_telemetry_gps1_status(_uart_fd, latitude, longitude, altitude);
}

bool GHSTTelemetry::send_gps2_status() {
    sensor_gps_s vehicle_gps_position;

    if (!_vehicle_gps_position_sub.update(&vehicle_gps_position)) {
        return false;
    }

    uint16_t ground_speed  = (uint16_t)(vehicle_gps_position.vel_d_m_s / 3.6f * 10.f);
    uint16_t ground_course = (uint16_t)(math::degrees(vehicle_gps_position.cog_rad) * 100.f);
    uint8_t  num_sats      = vehicle_gps_position.satellites_used;

    // TBD: Can these be computed in a RC telemetry driver?
    uint16_t home_dist = 0;
    uint16_t home_dir  = 0;
    uint8_t  flags     = 0;

    return ghst_send_telemetry_gps2_status(_uart_fd, ground_speed, ground_course, num_sats, home_dist, home_dir, flags);
}
