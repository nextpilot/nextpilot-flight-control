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
 * @file PAW3902.hpp
 *
 * Driver for the PAW3902JF-TXQT: Optical Motion Tracking Chip
 */

#pragma once

#include "PixArt_PAW3902_Registers.hpp"

#include <drivers/drv_hrt.h>
#include <drivers/device/spi.h>
#include <conversion/rotation.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/atomic.h>
#include <px4_platform_common/i2c_spi_buses.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_optical_flow.h>

using namespace time_literals;
using namespace PixArt_PAW3902;

#define DIR_WRITE(a) ((a) | Bit7)
#define DIR_READ(a)  ((a) & 0x7F)

class PAW3902 : public device::SPI, public I2CSPIDriver<PAW3902> {
public:
    PAW3902(const I2CSPIDriverConfig &config);
    virtual ~PAW3902();

    static void print_usage();

    void RunImpl();

    int  init() override;
    void print_status() override;

private:
    void exit_and_cleanup() override;

    int probe() override;

    bool Reset();
    bool Configure();

    static int DataReadyInterruptCallback(int irq, void *context, void *arg);
    void       DataReady();
    bool       DataReadyInterruptConfigure();
    bool       DataReadyInterruptDisable();

    uint8_t RegisterRead(uint8_t reg);
    void    RegisterWrite(uint8_t reg, uint8_t data);

    void ConfigureModeBright();
    void ConfigureModeLowLight();
    void ConfigureModeSuperLowLight();
    void EnableLed();

    enum class STATE : uint8_t {
        RESET,
        WAIT_FOR_RESET,
        CONFIGURE,
        READ,
    } _state{STATE::RESET};

    uORB::PublicationMulti<sensor_optical_flow_s> _sensor_optical_flow_pub{ORB_ID(sensor_optical_flow)};

    const spi_drdy_gpio_t _drdy_gpio;

    matrix::Dcmf _rotation;

    perf_counter_t _bad_register_perf{perf_alloc(PC_COUNT, MODULE_NAME ": bad register")};
    perf_counter_t _bad_transfer_perf{perf_alloc(PC_COUNT, MODULE_NAME ": bad transfer")};
    perf_counter_t _reset_perf{perf_alloc(PC_COUNT, MODULE_NAME ": reset")};
    perf_counter_t _false_motion_perf{perf_alloc(PC_COUNT, MODULE_NAME ": false motion report")};
    perf_counter_t _mode_change_bright_perf{perf_alloc(PC_COUNT, MODULE_NAME ": mode change bright (0)")};
    perf_counter_t _mode_change_low_light_perf{perf_alloc(PC_COUNT, MODULE_NAME ": mode change low light (1)")};
    perf_counter_t _mode_change_super_low_light_perf{perf_alloc(PC_COUNT, MODULE_NAME ": mode change super low light (2)")};
    perf_counter_t _no_motion_interrupt_perf{nullptr};

    hrt_abstime _reset_timestamp{0};
    hrt_abstime _last_publish{0};
    hrt_abstime _last_motion{0};

    int16_t  _delta_x_raw_prev{0};
    int16_t  _delta_y_raw_prev{0};
    uint16_t _shutter_prev{0};
    uint8_t  _quality_prev{0};
    uint8_t  _raw_data_sum_prev{0};

    int _failure_count{0};
    int _discard_reading{0};

    px4::atomic<hrt_abstime> _drdy_timestamp_sample{0};
    bool                     _motion_interrupt_enabled{false};

    uint32_t                  _scheduled_interval_us{SAMPLE_INTERVAL_MODE_0 / 2};
    static constexpr uint32_t kBackupScheduleIntervalUs{SAMPLE_INTERVAL_MODE_2}; // longest expected interval

    Mode _mode{Mode::LowLight};

    int _bright_to_low_counter{0};
    int _low_to_superlow_counter{0};
    int _low_to_bright_counter{0};
    int _superlow_to_low_counter{0};

    hrt_abstime _last_write_time{0};
    hrt_abstime _last_read_time{0};
};
