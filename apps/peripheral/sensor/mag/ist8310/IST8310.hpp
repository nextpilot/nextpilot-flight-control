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
 * @file IST8310.hpp
 *
 * Driver for the iSentek IST8310 connected via I2C.
 *
 */

#pragma once

#include "iSentek_IST8310_registers.hpp"

#include <drivers/drv_hrt.h>
#include <lib/drivers/device/i2c.h>
#include <lib/drivers/magnetometer/PX4Magnetometer.hpp>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/i2c_spi_buses.h>

using namespace iSentek_IST8310;

class IST8310 : public device::I2C, public I2CSPIDriver<IST8310> {
public:
    IST8310(const I2CSPIDriverConfig &config);
    ~IST8310() override;

    static void print_usage();

    void RunImpl();

    int  init() override;
    void print_status() override;

private:
    // Sensor Configuration
    struct register_config_t {
        Register reg;
        uint8_t  set_bits{0};
        uint8_t  clear_bits{0};
    };

    int probe() override;

    bool Reset();

    bool Configure();

    bool RegisterCheck(const register_config_t &reg_cfg);

    uint8_t RegisterRead(Register reg);
    void    RegisterWrite(Register reg, uint8_t value);
    void    RegisterSetAndClearBits(Register reg, uint8_t setbits, uint8_t clearbits);

    PX4Magnetometer _px4_mag;

    perf_counter_t _bad_register_perf{perf_alloc(PC_COUNT, MODULE_NAME ": bad register")};
    perf_counter_t _bad_transfer_perf{perf_alloc(PC_COUNT, MODULE_NAME ": bad transfer")};
    perf_counter_t _reset_perf{perf_alloc(PC_COUNT, MODULE_NAME ": reset")};

    hrt_abstime _reset_timestamp{0};
    hrt_abstime _last_config_check_timestamp{0};
    int         _failure_count{0};

    enum class STATE : uint8_t {
        RESET,
        WAIT_FOR_RESET,
        CONFIGURE,
        MEASURE,
        READ,
    } _state{STATE::RESET};

    uint8_t                  _checked_register{0};
    static constexpr uint8_t size_register_cfg{4};
    register_config_t        _register_cfg[size_register_cfg]{
        // Register               | Set bits, Clear bits
        {Register::CNTL2, 0, CNTL2_BIT::SRST},
        {Register::CNTL3, CNTL3_BIT::Z_16BIT | CNTL3_BIT::Y_16BIT | CNTL3_BIT::X_16BIT, 0},
        {Register::AVGCNTL, AVGCNTL_BIT::Y_16TIMES_SET | AVGCNTL_BIT::XZ_16TIMES_SET, AVGCNTL_BIT::Y_16TIMES_CLEAR | AVGCNTL_BIT::XZ_16TIMES_CLEAR},
        {Register::PDCNTL, PDCNTL_BIT::PULSE_NORMAL, 0},
    };
};
