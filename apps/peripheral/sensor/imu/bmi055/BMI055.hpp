/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __BMI055_H__
#define __BMI055_H__


#pragma once

#include <drivers/drv_hrt.h>
#include <lib/drivers/device/spi.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/i2c_spi_buses.h>

static constexpr int16_t combine(uint8_t msb, uint8_t lsb) {
    return (msb << 8u) | lsb;
}

class BMI055 : public device::SPI, public I2CSPIDriver<BMI055> {
public:
    BMI055(const I2CSPIDriverConfig &config);

    virtual ~BMI055() = default;

    static I2CSPIDriverBase *instantiate(const I2CSPIDriverConfig &config, int runtime_instance);
    static void              print_usage();

    virtual void RunImpl() = 0;

    int          init() override;
    virtual void print_status() = 0;

protected:
    bool Reset();

    const spi_drdy_gpio_t _drdy_gpio;

    hrt_abstime _reset_timestamp{0};
    hrt_abstime _last_config_check_timestamp{0};
    hrt_abstime _temperature_update_timestamp{0};
    int         _failure_count{0};

    px4::atomic<hrt_abstime> _drdy_timestamp_sample{0};
    bool                     _data_ready_interrupt_enabled{false};

    enum class STATE : uint8_t {
        RESET,
        WAIT_FOR_RESET,
        CONFIGURE,
        FIFO_READ,
    } _state{STATE::RESET};

    uint16_t _fifo_empty_interval_us{2500}; // 2500 us / 400 Hz transfer interval
};

#endif // __BMI055_H__
