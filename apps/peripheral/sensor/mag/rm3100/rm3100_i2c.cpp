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
 * @file rm3100_i2c.cpp
 *
 * I2C interface for RM3100
 */

#include <px4_platform_common/px4_config.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/device/i2c.h>

#include "board_config.h"
#include "rm3100.h"

class RM3100_I2C : public device::I2C {
public:
    RM3100_I2C(int bus, int bus_frequency);
    ~RM3100_I2C() override = default;

    int read(unsigned address, void *data, unsigned count) override;
    int write(unsigned address, void *data, unsigned count) override;

protected:
    int probe() override;
};

device::Device *
RM3100_I2C_interface(int bus, int bus_frequency);

device::Device *
RM3100_I2C_interface(int bus, int bus_frequency) {
    return new RM3100_I2C(bus, bus_frequency);
}

RM3100_I2C::RM3100_I2C(int bus, int bus_frequency) :
    I2C(DRV_MAG_DEVTYPE_RM3100, MODULE_NAME, bus, RM3100_ADDRESS, bus_frequency) {
}

int RM3100_I2C::probe() {
    uint8_t data = 0;

    if (read(ADDR_REVID, &data, 1)) {
        DEVICE_DEBUG("RM3100 read_reg fail");
        return -EIO;
    }

    if (data != RM3100_REVID) {
        DEVICE_DEBUG("RM3100 bad ID: %02x", data);
        return -EIO;
    }

    _retries = 1;

    return OK;
}

int RM3100_I2C::read(unsigned address, void *data, unsigned count) {
    uint8_t cmd = address;
    int     ret;

    /* We need a first transfer where we write the register to read */
    ret = transfer(&cmd, 1, nullptr, 0);

    if (ret != OK) {
        return ret;
    }

    /* Now we read the previously selected register */
    ret = transfer(nullptr, 0, (uint8_t *)data, count);

    return ret;
}

int RM3100_I2C::write(unsigned address, void *data, unsigned count) {
    uint8_t buf[32];

    if (sizeof(buf) < (count + 1)) {
        return -EIO;
    }

    buf[0] = address;
    memcpy(&buf[1], data, count);

    return transfer(&buf[0], count + 1, nullptr, 0);
}
