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
 * @file Bosch_BMM150_registers.hpp
 *
 * Bosch BMM150 registers.
 *
 */

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

namespace Bosch_BMM150 {
static constexpr uint32_t I2C_SPEED           = 400 * 1000; // 400 kHz I2C serial interface
static constexpr uint8_t  I2C_ADDRESS_DEFAULT = 0x10;

static constexpr uint8_t chip_identification_number = 0x32;

static constexpr int16_t OVERFLOW_XYAXES = -4096;
static constexpr int16_t OVERFLOW_ZAXIS  = -16384;

enum class Register : uint8_t {
    CHIP_ID = 0x40,       // magnetometer chip identification number

    DATAX_LSB     = 0x42, // 5-bit LSB of x-axis magnetic field data
    DATAX_MSB     = 0x43, // 8-bit MSB of x-axis magnetic field data
    DATAY_LSB     = 0x44, // 5-bit LSB of y-axis magnetic field data
    DATAY_MSB     = 0x45, // 8-bit MSB of y-axis magnetic field data
    DATAZ_LSB     = 0x46, // 7-bit LSB of z-axis magnetic field data
    DATAZ_MSB     = 0x47, // 8-bit MSB of z-axis magnetic field data
    RHALL_LSB     = 0x48, // 6-bit LSB of hall resistance
    RHALL_MSB     = 0x49, // 8-bit MSB of hall resistance
    STATUS        = 0x4A, // Status register
    POWER_CONTROL = 0x4B, // power control, soft reset and interface SPI mode selection
    OP_MODE       = 0x4C, // operation mode, output data rate and self-test

    REPXY = 0x51,         // number of repetitions for x/y-axis
    REPZ  = 0x52,         // number of repetitions for z-axis

    DIG_X1 = 0x5D,

    DIG_Z4_LSB = 0x62,

    DIG_Z2_LSB = 0x68,
};

// POWER_CONTROL
enum POWER_CONTROL_BIT : uint8_t {
    SoftReset    = Bit7 | Bit1, // soft reset trigger bits.
    PowerControl = Bit0,        // When set to “0”, suspend mode is selected
};

// OP_MODE
enum OP_MODE_BIT : uint8_t {
    // 5:3 Data rate control
    ODR_20HZ_SET   = Bit5 | Bit3,
    ODR_20HZ_CLEAR = Bit4,

    // 2:1 Operation mode control
    Opmode_Sleep = Bit2 | Bit1, // Sleep mode
    Self_Test    = Bit0,
};

// STATUS
enum STATUS_BIT : uint8_t {
    Overflow = Bit6, // one or more axes exceeded maximum range of the device
};

// REPXY
enum REPXY_BIT : uint8_t {
    // high accurary preset nXY = 47, REPXY = 0x17 = 0b0001'0111
    XY_HA_SET   = Bit4 | Bit2 | Bit1 | Bit0,
    XY_HA_CLEAR = Bit7 | Bit6 | Bit5 | Bit3,
};

// REPZ
enum REPZ_BIT : uint8_t {
    // high accurary preset nZ = 83, REPZ = 0x52 = 0b0101'0010
    Z_HA_SET   = Bit6 | Bit4 | Bit1,
    Z_HA_CLEAR = Bit7 | Bit5 | Bit3 | Bit2 | Bit0,
};

} // namespace Bosch_BMM150
