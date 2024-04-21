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

#   ifdef __PX4_LINUX

#      include <linux/i2c.h>
#      include <linux/i2c-dev.h>

#      include <px4_platform_common/i2c_spi_buses.h>

namespace device {

I2C::I2C(uint8_t device_type, const char *name, const int bus, const uint16_t address, const uint32_t frequency) :
    CDev(name, nullptr) {
    // fill in _device_id fields for a I2C device
    _device_id.devid_s.devtype  = device_type;
    _device_id.devid_s.bus_type = DeviceBusType_I2C;
    _device_id.devid_s.bus      = bus;
    _device_id.devid_s.address  = address;
}

I2C::I2C(const I2CSPIDriverConfig &config) :
    I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency) {
}

I2C::~I2C() {
    if (_fd >= 0) {
        ::close(_fd);
        _fd = -1;
    }
}

int I2C::init() {
    int ret = PX4_ERROR;

    // Open the actual I2C device
    char dev_path[16]{};
    snprintf(dev_path, sizeof(dev_path), "/dev/i2c-%i", get_device_bus());
    _fd = ::open(dev_path, O_RDWR);

    if (_fd < 0) {
        DEVICE_DEBUG("failed to init I2C");
        ret = -ENOENT;
        goto out;
    }

    // call the probe function to check whether the device is present
    ret = probe();

    if (ret != OK) {
        DEVICE_DEBUG("probe failed");
        goto out;
    }

    // do base class init, which will create device node, etc
    ret = CDev::init();

    if (ret != OK) {
        DEVICE_DEBUG("cdev init failed");
        goto out;
    }

    // tell the world where we are
    DEVICE_DEBUG("on I2C bus %d at 0x%02x", get_device_bus(), get_device_address());

out:

    if ((ret != OK) && !(_fd < 0)) {
        ::close(_fd);
        _fd = -1;
    }

    return ret;
}

int I2C::transfer(const uint8_t *send, const unsigned send_len, uint8_t *recv, const unsigned recv_len) {
    int      ret         = PX4_ERROR;
    unsigned retry_count = 0;

    if (_fd < 0) {
        PX4_ERR("I2C device not opened");
        return PX4_ERROR;
    }

    do {
        DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

        unsigned       msgs = 0;
        struct i2c_msg msgv[2]{};

        if (send_len > 0) {
            msgv[msgs].addr  = get_device_address();
            msgv[msgs].flags = 0;
            msgv[msgs].buf   = const_cast<uint8_t *>(send);
            msgv[msgs].len   = send_len;
            msgs++;
        }

        if (recv_len > 0) {
            msgv[msgs].addr  = get_device_address();
            msgv[msgs].flags = I2C_M_RD;
            msgv[msgs].buf   = recv;
            msgv[msgs].len   = recv_len;
            msgs++;
        }

        if (msgs == 0) {
            return -EINVAL;
        }

        i2c_rdwr_ioctl_data packets{};
        packets.msgs  = msgv;
        packets.nmsgs = msgs;

        int ret_ioctl = ::ioctl(_fd, I2C_RDWR, (unsigned long)&packets);

        if (ret_ioctl == -1) {
            DEVICE_DEBUG("I2C transfer failed");
            ret = PX4_ERROR;

        } else {
            // success
            ret = PX4_OK;
            break;
        }

    } while (retry_count++ < _retries);

    return ret;
}

} // namespace device

#   endif // __PX4_LINUX

#endif    // CONFIG_I2C
