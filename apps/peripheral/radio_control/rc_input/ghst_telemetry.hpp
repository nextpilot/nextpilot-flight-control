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

#pragma once

#include <uORB/Subscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/sensor_gps.h>
#include <drivers/drv_hrt.h>

/**
 * High-level class that handles sending of GHST telemetry data
 */
class GHSTTelemetry {
public:
    /**
	 * @param uart_fd file descriptor for the UART to use. It is expected to be configured
	 * already.
	 */
    explicit GHSTTelemetry(int uart_fd);

    ~GHSTTelemetry() = default;

    /**
	 * Send telemetry data. Call this regularly (i.e. at 100Hz), it will automatically
	 * limit the sending rate.
	 * @return true if new data sent
	 */
    bool update(const hrt_abstime &now);

private:
    bool send_battery_status();
    bool send_gps1_status();
    bool send_gps2_status();

    uORB::Subscription _vehicle_gps_position_sub{ORB_ID(vehicle_gps_position)};
    uORB::Subscription _battery_status_sub{ORB_ID(battery_status)};

    int         _uart_fd;
    hrt_abstime _last_update{0U};
    uint32_t    _next_type{0U};

    static constexpr uint32_t NUM_DATA_TYPES{3U};  // number of different telemetry data types
    static constexpr uint32_t UPDATE_RATE_HZ{10U}; // update rate [Hz]

    // Factors that should be applied to get correct values
    static constexpr float FACTOR_VOLTS_TO_10MV{100.0F};
    static constexpr float FACTOR_AMPS_TO_10MA{100.0F};
    static constexpr float FACTOR_MAH_TO_10MAH{0.1F};
};
