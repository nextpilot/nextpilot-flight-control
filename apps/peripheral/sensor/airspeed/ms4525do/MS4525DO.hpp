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
 *
 * Driver for the MEAS Spec series MS4525DO connected via I2C.
 *
 * Supported sensors:
 *
 *    - MS4525DO
 *
 * Interface application notes:
 *
 *    - Interfacing to MEAS Digital Pressure Modules (http://www.meas-spec.com/downloads/Interfacing_to_MEAS_Digital_Pressure_Modules.pdf)
 */

#pragma once

#include <drivers/drv_hrt.h>
#include <lib/drivers/device/i2c.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/i2c_spi_buses.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/differential_pressure.h>

static constexpr uint32_t I2C_SPEED           = 100 * 1000; // 100 kHz I2C serial interface
static constexpr uint8_t  I2C_ADDRESS_DEFAULT = 0x28;

/* Register address */
#define ADDR_READ_MR 0x00

class MS4525DO : public device::I2C, public I2CSPIDriver<MS4525DO> {
public:
    MS4525DO(const I2CSPIDriverConfig &config);
    ~MS4525DO() override;

    static void print_usage();

    void RunImpl();

    int  init() override;
    void print_status() override;

private:
    int probe() override;

    enum class STATE : uint8_t {
        MEASURE,
        READ,
    } _state{STATE::MEASURE};

    enum class STATUS : uint8_t {
        Normal_Operation = 0b00, // 0: Normal Operation. Good Data Packet
        Reserved         = 0b01, // 1: Reserved
        Stale_Data       = 0b10, // 2: Stale Data. Data has been fetched since last measurement cycle.
        Fault_Detected   = 0b11, // 3: Fault Detected
    };

    hrt_abstime _timestamp_sample{0};

    uORB::PublicationMulti<differential_pressure_s> _differential_pressure_pub{ORB_ID(differential_pressure)};

    perf_counter_t _sample_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": read")};
    perf_counter_t _comms_errors{perf_alloc(PC_COUNT, MODULE_NAME ": communication errors")};
    perf_counter_t _fault_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": fault detected")};
};
