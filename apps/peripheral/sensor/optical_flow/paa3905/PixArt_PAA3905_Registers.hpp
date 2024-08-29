/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <cstdint>

// TODO: move to a central header
static constexpr uint8_t Bit0 = (1 << 0);
static constexpr uint8_t Bit1 = (1 << 1);
static constexpr uint8_t Bit2 = (1 << 2);
static constexpr uint8_t Bit3 = (1 << 3);
static constexpr uint8_t Bit4 = (1 << 4);
static constexpr uint8_t Bit5 = (1 << 5);
static constexpr uint8_t Bit6 = (1 << 6);
static constexpr uint8_t Bit7 = (1 << 7);

namespace PixArt_PAA3905 {

static constexpr uint8_t PRODUCT_ID         = 0xA2;
static constexpr uint8_t REVISION_ID        = 0x00;
static constexpr uint8_t PRODUCT_ID_INVERSE = 0x5D;

static constexpr uint32_t SAMPLE_INTERVAL_MODE_0{1000000 / 126}; // 126 fps
static constexpr uint32_t SAMPLE_INTERVAL_MODE_1{1000000 / 126}; // 126 fps
static constexpr uint32_t SAMPLE_INTERVAL_MODE_2{1000000 / 50};  // 50 fps

static constexpr uint32_t SPI_SPEED = 2 * 1000 * 1000;           // 2MHz SPI serial interface

// Various time delays
static constexpr uint32_t TIME_TSWW_us      = 11; // SPI Time Between Write Commands (actually 10.5us)
static constexpr uint32_t TIME_TSWR_us      = 6;  // SPI Time Between Write and Read Commands
static constexpr uint32_t TIME_TSRW_TSRR_us = 2;  // SPI Time Between Read And Subsequent Commands (actually 1.5us)
static constexpr uint32_t TIME_TSRAD_us     = 2;  // SPI Read Address-Data Delay

enum Register : uint8_t {
    Product_ID      = 0x00,
    Revision_ID     = 0x01,
    Motion          = 0x02,
    Delta_X_L       = 0x03,
    Delta_X_H       = 0x04,
    Delta_Y_L       = 0x05,
    Delta_Y_H       = 0x06,
    Squal           = 0x07,
    RawData_Sum     = 0x08,
    Maximum_RawData = 0x09,
    Minimum_RawData = 0x0A,
    Shutter_Lower   = 0x0B,
    Shutter_Middle  = 0x0C,
    Shutter_Upper   = 0x0D,

    Observation  = 0x15,
    Motion_Burst = 0x16,

    Power_Up_Reset = 0x3A,
    Shutdown       = 0x3B,

    Resolution = 0x4E,

    Inverse_Product_ID = 0x5F,
};

enum Motion_Bit : uint8_t {
    MotionOccurred     = Bit7, // Motion since last report
    ChallengingSurface = Bit0, // Challenging surface is detected
};

enum Observation_Bit : uint8_t {
    // Bit [7:6]
    AMS_mode_0 = 0,
    AMS_mode_1 = Bit6,
    AMS_mode_2 = Bit7,

    // Bit [5:0]
    WorkingCorrectly = 0x3F,
};

enum class Mode {
    Bright        = 0,
    LowLight      = 1,
    SuperLowLight = 2,
};

struct BURST_TRANSFER {
    uint8_t Motion;
    uint8_t Observation;
    uint8_t Delta_X_L;
    uint8_t Delta_X_H;
    uint8_t Delta_Y_L;
    uint8_t Delta_Y_H;
    uint8_t Reserved;
    uint8_t SQUAL;
    uint8_t RawData_Sum;
    uint8_t Maximum_RawData;
    uint8_t Minimum_RawData;
    uint8_t Shutter_Upper;
    uint8_t Shutter_Middle;
    uint8_t Shutter_Lower;
};

} // namespace PixArt_PAA3905
