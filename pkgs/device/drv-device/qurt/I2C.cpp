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
 * @file I2C.cpp
 *
 * Base class for devices attached via the I2C bus.
 *
 * @todo Bus frequency changes; currently we do nothing with the value
 *       that is supplied.  Should we just depend on the bus knowing?
 */

#include "I2C.hpp"

#if defined(CONFIG_I2C)


#   include <px4_platform_common/i2c_spi_buses.h>

namespace device {

I2C::_config_i2c_bus_func_t  I2C::_config_i2c_bus  = NULL;
I2C::_set_i2c_address_func_t I2C::_set_i2c_address = NULL;
I2C::_i2c_transfer_func_t    I2C::_i2c_transfer    = NULL;

pthread_mutex_t I2C::_mutex = PTHREAD_MUTEX_INITIALIZER;

I2C::I2C(uint8_t device_type, const char *name, const int bus, const uint16_t address, const uint32_t frequency) :
    CDev(name, nullptr),
    _frequency(frequency / 1000) {
    _device_id.devid = 0;

    // fill in _device_id fields for a I2C device
    _device_id.devid_s.devtype  = device_type;
    _device_id.devid_s.bus_type = DeviceBusType_I2C;
    _device_id.devid_s.bus      = bus;
    _device_id.devid_s.address  = address;

    PX4_INFO("*** I2C Device ID 0x%x %d", _device_id.devid, _device_id.devid);
}

I2C::I2C(const I2CSPIDriverConfig &config) :
    I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency) {
}

I2C::~I2C() {
}

int I2C::init() {
    int ret = PX4_ERROR;

    if (_config_i2c_bus == NULL) {
        PX4_ERR("NULL i2c init function");
        goto out;
    }

    pthread_mutex_lock(&_mutex);
    // Open the actual I2C device
    _i2c_fd = _config_i2c_bus(get_device_bus(), get_device_address(), _frequency);
    pthread_mutex_unlock(&_mutex);

    if (_i2c_fd == PX4_ERROR) {
        PX4_ERR("i2c init failed");
        goto out;
    }

    // call the probe function to check whether the device is present
    ret = probe();

    if (ret != OK) {
        PX4_ERR("i2c probe failed");
        goto out;
    }

    // do base class init, which will create device node, etc
    ret = CDev::init();

    if (ret != OK) {
        PX4_ERR("i2c cdev init failed");
        goto out;
    }

    // tell the world where we are
    // PX4_INFO("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

out:

    return ret;
}

void I2C::set_device_address(int address) {
    if ((_i2c_fd != PX4_ERROR) && (_set_i2c_address != NULL)) {
        PX4_INFO("Set i2c address 0x%x, fd %d", address, _i2c_fd);

        pthread_mutex_lock(&_mutex);
        _set_i2c_address(_i2c_fd, address);
        pthread_mutex_unlock(&_mutex);

        Device::set_device_address(address);
    }
}

int I2C::transfer(const uint8_t *send, const unsigned send_len, uint8_t *recv, const unsigned recv_len) {
    int      ret         = PX4_ERROR;
    unsigned retry_count = 1;

    if ((_i2c_fd != PX4_ERROR) && (_i2c_transfer != NULL)) {
        do {
            // PX4_INFO("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

            pthread_mutex_lock(&_mutex);
            ret = _i2c_transfer(_i2c_fd, send, send_len, recv, recv_len);
            pthread_mutex_unlock(&_mutex);

            if (ret != PX4_ERROR) {
                break;
            }

            usleep(1000);

        } while (retry_count++ < _retries);
    }

    return ret;
}

} // namespace device

#endif
