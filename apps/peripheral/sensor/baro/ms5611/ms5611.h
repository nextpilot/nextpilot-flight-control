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
 * @file ms5611.h
 *
 * Shared defines for the ms5611 driver.
 */

#pragma once

#include "board_config.h"

#include <string.h>

#if defined(CONFIG_I2C)
#   include <drivers/device/i2c.h>
#endif // CONFIG_I2C

#include <drivers/device/device.h>
#include <drivers/device/spi.h>
#include <lib/cdev/CDev.hpp>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <systemlib/err.h>

#define ADDR_RESET_CMD  0x1E /* write to this address to reset chip */
#define ADDR_PROM_SETUP 0xA0 /* address of 8x 2 bytes factory and calibration data */

/* interface ioctls */
#define IOCTL_RESET      2
#define IOCTL_MEASURE    3

#define MS5611_ADDRESS_1 0x76 /* address select pins pulled high (PX4FMU series v1.6+) */
#define MS5611_ADDRESS_2 0x77 /* address select pins pulled low (PX4FMU prototypes) */

namespace ms5611 {

/**
 * Calibration PROM as reported by the device.
 */
#pragma pack(push, 1)

struct prom_s {
    uint16_t factory_setup;
    uint16_t c1_pressure_sens;
    uint16_t c2_pressure_offset;
    uint16_t c3_temp_coeff_pres_sens;
    uint16_t c4_temp_coeff_pres_offset;
    uint16_t c5_reference_temp;
    uint16_t c6_temp_coeff_temp;
    uint16_t serial_and_crc;
};

/**
 * Grody hack for crc4()
 */
union prom_u {
    uint16_t c[8];
    prom_s   s;
};

#pragma pack(pop)

extern bool crc4(uint16_t *n_prom);

} // namespace ms5611

/* interface factories */
extern device::Device *MS5611_spi_interface(ms5611::prom_u &prom_buf, uint32_t devid, uint8_t busnum, int bus_frequency,
                                            spi_mode_e spi_mode);

#if defined(CONFIG_I2C)
extern device::Device *MS5611_i2c_interface(ms5611::prom_u &prom_buf, uint32_t devid, uint8_t busnum,
                                            int bus_frequency);
#endif // CONFIG_I2C

typedef device::Device *(*MS5611_constructor)(ms5611::prom_u &prom_buf, uint32_t devid, uint8_t busnum);
