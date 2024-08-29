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
 * @file rm3100_spi.cpp
 *
 * SPI interface for RM3100
 */

#include <px4_platform_common/px4_config.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/device/spi.h>

#include "board_config.h"
#include "rm3100.h"

/* SPI protocol address bits */
#define DIR_READ  (1 << 7)
#define DIR_WRITE (0 << 7)

class RM3100_SPI : public device::SPI {
public:
    RM3100_SPI(int bus, uint32_t devid, int bus_frequency, spi_mode_e spi_mode);
    virtual ~RM3100_SPI() = default;

    virtual int init();
    virtual int read(unsigned address, void *data, unsigned count);
    virtual int write(unsigned address, void *data, unsigned count);
};

device::Device *
RM3100_SPI_interface(int bus, uint32_t devid, int bus_frequency, spi_mode_e spi_mode);

device::Device *
RM3100_SPI_interface(int bus, uint32_t devid, int bus_frequency, spi_mode_e spi_mode) {
    return new RM3100_SPI(bus, devid, bus_frequency, spi_mode);
}

RM3100_SPI::RM3100_SPI(int bus, uint32_t devid, int bus_frequency, spi_mode_e spi_mode) :
    SPI(DRV_MAG_DEVTYPE_RM3100, MODULE_NAME, bus, devid, spi_mode, bus_frequency) {
}

int RM3100_SPI::init() {
    int ret;

    ret = SPI::init();

    if (ret != OK) {
        DEVICE_DEBUG("SPI init failed");
        return -EIO;
    }

    // Read REV_ID value
    uint8_t data = 0;

    if (read(ADDR_REVID, &data, 1)) {
        DEVICE_DEBUG("RM3100 read_reg fail");
    }

    if (data != RM3100_REVID) {
        DEVICE_DEBUG("RM3100 ID: %02x", data);
        return -EIO;
    }

    return OK;
}

int RM3100_SPI::read(unsigned address, void *data, unsigned count) {
    uint8_t buf[32];

    if (sizeof(buf) < (count + 1)) {
        return -EIO;
    }

    buf[0] = address | DIR_READ;

    int ret = transfer(&buf[0], &buf[0], count + 1);
    memcpy(data, &buf[1], count);
    return ret;
}

int RM3100_SPI::write(unsigned address, void *data, unsigned count) {
    uint8_t buf[32];

    if (sizeof(buf) < (count + 1)) {
        return -EIO;
    }

    buf[0] = address | DIR_WRITE;
    memcpy(&buf[1], data, count);

    return transfer(&buf[0], &buf[0], count + 1);
}
