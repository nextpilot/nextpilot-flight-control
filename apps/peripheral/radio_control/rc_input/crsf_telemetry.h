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
 * @file crsf_telemetry.h
 *
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#pragma once

#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/vehicle_status.h>
#include <drivers/drv_hrt.h>

#include <matrix/math.hpp>
#include <mathlib/mathlib.h>

using namespace time_literals;

/**
 * High-level class that handles sending of CRSF telemetry data
 */
class CRSFTelemetry {
public:
    /**
	 * @param uart_fd file descriptor for the UART to use. It is expected to be configured
	 * already.
	 */
    CRSFTelemetry(int uart_fd);

    ~CRSFTelemetry() = default;

    /**
	 * Send telemetry data. Call this regularly (i.e. at 100Hz), it will automatically
	 * limit the sending rate.
	 * @return true if new data sent
	 */
    bool update(const hrt_abstime &now);

private:
    bool send_battery();
    bool send_gps();
    bool send_attitude();
    bool send_flight_mode();

    uORB::Subscription _vehicle_gps_position_sub{ORB_ID(vehicle_gps_position)};
    uORB::Subscription _battery_status_sub{ORB_ID(battery_status)};
    uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    hrt_abstime _last_update{0};

    static constexpr int num_data_types{4}; ///< number of different telemetry data types
    int                  _next_type{0};

    int _uart_fd;
};
