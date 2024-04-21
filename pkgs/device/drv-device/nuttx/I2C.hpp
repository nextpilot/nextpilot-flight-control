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

/**
 * @file I2C.hpp
 *
 * Base class for devices connected via I2C.
 */

#include "../CDev.hpp"
#include <px4_platform_common/i2c.h>

#if defined(CONFIG_I2C)

#   include <nuttx/i2c/i2c_master.h>

struct I2CSPIDriverConfig;

namespace device __EXPORT {

/**
 * Abstract class for character device on I2C
 */
class __EXPORT I2C : public CDev {
public:
    // no copy, assignment, move, move assignment
    I2C(const I2C &)            = delete;
    I2C &operator=(const I2C &) = delete;
    I2C(I2C &&)                 = delete;
    I2C &operator=(I2C &&)      = delete;

    virtual int init() override;

    static int set_bus_clock(unsigned bus, unsigned clock_hz);

protected:
    /**
	 * The number of times a read or write operation will be retried on
	 * error.
	 */
    uint8_t _retries{0};

    /**
	 * @ Constructor
	 *
	 * @param device_type	The device type (see drv_sensor.h)
	 * @param name		Driver name
	 * @param bus		I2C bus on which the device lives
	 * @param address	I2C bus address, or zero if set_address will be used
	 * @param frequency	I2C bus frequency for the device (currently not used)
	 */
    I2C(uint8_t device_type, const char *name, const int bus, const uint16_t address, const uint32_t frequency);
    I2C(const I2CSPIDriverConfig &config);
    virtual ~I2C();

    /**
	 * Check for the presence of the device on the bus.
	 */
    virtual int probe() {
        return PX4_OK;
    }

    /**
	 * Perform an I2C transaction to the device.
	 *
	 * At least one of send_len and recv_len must be non-zero.
	 *
	 * @param send		Pointer to bytes to send.
	 * @param send_len	Number of bytes to send.
	 * @param recv		Pointer to buffer for bytes received.
	 * @param recv_len	Number of bytes to receive.
	 * @return		OK if the transfer was successful, -errno
	 *			otherwise.
	 */
    int transfer(const uint8_t *send, const unsigned send_len, uint8_t *recv, const unsigned recv_len);

    bool external() const override {
        return px4_i2c_device_external(_device_id.devid);
    }

private:
    static unsigned int _bus_clocks[PX4_NUMBER_I2C_BUSES];

    const uint32_t _frequency;
    i2c_master_s  *_dev{nullptr};
};

} // namespace device __EXPORT

#endif // CONFIG_I2C
