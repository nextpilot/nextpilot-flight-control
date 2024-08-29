/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "crsf_telemetry.h"
#include <lib/rc/crsf.h>

CRSFTelemetry::CRSFTelemetry(int uart_fd) :
    _uart_fd(uart_fd) {
}

bool CRSFTelemetry::update(const hrt_abstime &now) {
    const int update_rate_hz = 10;

    if (now - _last_update <= 1_s / (update_rate_hz * num_data_types)) {
        return false;
    }

    bool sent = false;

    switch (_next_type) {
    case 0:
        sent = send_battery();
        break;

    case 1:
        sent = send_gps();
        break;

    case 2:
        sent = send_attitude();
        break;

    case 3:
        sent = send_flight_mode();
        break;
    }

    _last_update = now;
    _next_type   = (_next_type + 1) % num_data_types;

    return sent;
}

bool CRSFTelemetry::send_battery() {
    battery_status_s battery_status;

    if (!_battery_status_sub.update(&battery_status)) {
        return false;
    }

    uint16_t voltage   = battery_status.voltage_filtered_v * 10;
    uint16_t current   = battery_status.current_filtered_a * 10;
    int      fuel      = battery_status.discharged_mah;
    uint8_t  remaining = battery_status.remaining * 100;
    return crsf_send_telemetry_battery(_uart_fd, voltage, current, fuel, remaining);
}

bool CRSFTelemetry::send_gps() {
    sensor_gps_s vehicle_gps_position;

    if (!_vehicle_gps_position_sub.update(&vehicle_gps_position)) {
        return false;
    }

    int32_t  latitude       = vehicle_gps_position.lat;
    int32_t  longitude      = vehicle_gps_position.lon;
    uint16_t groundspeed    = vehicle_gps_position.vel_d_m_s / 3.6f * 10.f;
    uint16_t gps_heading    = math::degrees(vehicle_gps_position.cog_rad) * 100.f;
    uint16_t altitude       = vehicle_gps_position.alt + 1000;
    uint8_t  num_satellites = vehicle_gps_position.satellites_used;

    return crsf_send_telemetry_gps(_uart_fd, latitude, longitude, groundspeed,
                                   gps_heading, altitude, num_satellites);
}

bool CRSFTelemetry::send_attitude() {
    vehicle_attitude_s vehicle_attitude;

    if (!_vehicle_attitude_sub.update(&vehicle_attitude)) {
        return false;
    }

    matrix::Eulerf attitude = matrix::Quatf(vehicle_attitude.q);
    int16_t        pitch    = attitude(1) * 1e4f;
    int16_t        roll     = attitude(0) * 1e4f;
    int16_t        yaw      = attitude(2) * 1e4f;
    return crsf_send_telemetry_attitude(_uart_fd, pitch, roll, yaw);
}

bool CRSFTelemetry::send_flight_mode() {
    vehicle_status_s vehicle_status;

    if (!_vehicle_status_sub.update(&vehicle_status)) {
        return false;
    }

    const char *flight_mode = "(unknown)";

    switch (vehicle_status.nav_state) {
    case vehicle_status_s::NAVIGATION_STATE_MANUAL:
        flight_mode = "Manual";
        break;

    case vehicle_status_s::NAVIGATION_STATE_ALTCTL:
        flight_mode = "Altitude";
        break;

    case vehicle_status_s::NAVIGATION_STATE_POSCTL:
        flight_mode = "Position";
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_RTL:
        flight_mode = "Return";
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
        flight_mode = "Mission";
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER:
    case vehicle_status_s::NAVIGATION_STATE_DESCEND:
    case vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
    case vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
    case vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
    case vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
        flight_mode = "Auto";
        break;

    case vehicle_status_s::NAVIGATION_STATE_ACRO:
        flight_mode = "Acro";
        break;

    case vehicle_status_s::NAVIGATION_STATE_TERMINATION:
        flight_mode = "Terminate";
        break;

    case vehicle_status_s::NAVIGATION_STATE_OFFBOARD:
        flight_mode = "Offboard";
        break;

    case vehicle_status_s::NAVIGATION_STATE_STAB:
        flight_mode = "Stabilized";
        break;
    }

    return crsf_send_telemetry_flight_mode(_uart_fd, flight_mode);
}
