/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <board_config.h>

#include <px4_platform_common/i2c.h>

#if defined(CONFIG_I2C)

#   ifndef BOARD_OVERRIDE_I2C_BUS_EXTERNAL
bool px4_i2c_bus_external(int bus) {
    for (int i = 0; i < I2C_BUS_MAX_BUS_ITEMS; ++i) {
        if (px4_i2c_buses[i].bus == bus) {
            return px4_i2c_buses[i].is_external;
        }
    }

    return true;
}
#   endif // BOARD_OVERRIDE_I2C_BUS_EXTERNAL

#   ifndef BOARD_OVERRIDE_I2C_DEVICE_EXTERNAL
#      include <drivers/device/Device.hpp>

bool px4_i2c_device_external(const uint32_t device_id) {
    device::Device::DeviceId dev_id{};
    dev_id.devid = device_id;
    return px4_i2c_bus_external(dev_id.devid_s.bus);
}
#   endif // BOARD_OVERRIDE_I2C_DEVICE_EXTERNAL

bool I2CBusIterator::next() {
    while (++_index < I2C_BUS_MAX_BUS_ITEMS && px4_i2c_buses[_index].bus != -1) {
        const px4_i2c_bus_t &bus_data = px4_i2c_buses[_index];

        if (!board_has_bus(BOARD_I2C_BUS, bus_data.bus)) {
            continue;
        }

        switch (_filter) {
        case FilterType::All:
            if (_bus == bus_data.bus || _bus == -1) {
                return true;
            }

            break;

        case FilterType::InternalBus:
            if (!px4_i2c_bus_external(bus_data.bus)) {
                if (_bus == bus_data.bus || _bus == -1) {
                    return true;
                }
            }

            break;

        case FilterType::ExternalBus:
            if (px4_i2c_bus_external(bus_data.bus)) {
                ++_external_bus_counter;

                if (_bus == bus_data.bus || _bus == -1) {
                    return true;
                }
            }

            break;
        }
    }

    return false;
}

#endif // CONFIG_I2C
