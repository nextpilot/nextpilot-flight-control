/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "i2c.hpp"

namespace nextpilot::device {

I2C::I2C(uint8_t device_type, const char *name, const int bus, const uint16_t address, const uint32_t frequency) :
    Device(name, nullptr),
    _frequency(frequency) {
    // fill in _device_id fields for a I2C device
    _device_id.devid_s.devtype   = device_type;
    _device_id.devid_s.bus_type  = DeviceBusType_I2C;
    _device_id.devid_s.bus_index = bus;
    _device_id.devid_s.address   = address;
}

// I2C::I2C(const I2CSPIDriverConfig &config) :
//     I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency) {
// }

I2C::~I2C() {
    if (_dev) {
        // px4_i2cbus_uninitialize(_dev);
        _dev = nullptr;
    }
}

int I2C::init() {
    _dev = rt_device_find("");
    if (!_dev) {
        return -RT_ERROR;
    }

    if (probe() != RT_EOK) {
        return -RT_ERROR;
    }

    if (Device::init() != RT_EOK) {
        return -RT_ERROR;
    }

    return RT_EOK;
}

int I2C::tansfer(const uint8_t *send, const unsigned send_len, uint8_t *recv, const unsigned recv_len) {
}

} // namespace nextpilot::device
