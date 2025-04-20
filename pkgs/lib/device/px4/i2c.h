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

#include <board_config.h>

#if defined(CONFIG_I2C)

#   define I2C_BUS_MAX_BUS_ITEMS PX4_NUMBER_I2C_BUSES

struct px4_i2c_bus_t {
    int  bus{-1};     ///< physical bus number (1, ...) (-1 means this is unused)
    bool is_external; ///< static external configuration. Use px4_i2c_bus_external() to check if a bus is really external
};

__EXPORT extern const px4_i2c_bus_t px4_i2c_buses[I2C_BUS_MAX_BUS_ITEMS]; ///< board-specific I2C bus configuration

/**
 * runtime-check if a board has a specific bus as external.
 * This can be overridden by a board to add run-time checks.
 */
__EXPORT bool px4_i2c_bus_external(int bus);

/**
 * runtime-check if a board has a specific device as external.
 * This can be overridden by a board to add run-time checks.
 */
__EXPORT bool px4_i2c_device_external(const uint32_t device_id);

/**
 * @class I2CBusIterator
 * Iterate over configured I2C buses by the board
 */
class I2CBusIterator {
public:
    enum class FilterType {
        All,         ///< specific or all buses
        InternalBus, ///< specific or all internal buses
        ExternalBus, ///< specific or all external buses
    };

    /**
	 * @param bus specify bus: starts with 1, -1=all. Internal: arch-specific bus numbering is used,
	 *             external: n-th external bus
	 */
    I2CBusIterator(FilterType filter, int bus = -1) :
        _filter(filter), _bus(bus) {
    }

    bool next();

    const px4_i2c_bus_t &bus() const {
        return px4_i2c_buses[_index];
    }

    int externalBusIndex() const {
        return _external_bus_counter;
    }

    bool external() const {
        return px4_i2c_bus_external(_bus);
    }

private:
    const FilterType _filter;
    const int        _bus;
    int              _index{-1};
    int              _external_bus_counter{0};
};

#endif // CONFIG_I2C
