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
 * @file rm3100_main.cpp
 *
 * Driver for the RM3100 magnetometer connected via I2C or SPI.
 */

#include "rm3100.h"
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

I2CSPIDriverBase *RM3100::instantiate(const I2CSPIDriverConfig &config, int runtime_instance) {
    device::Device *interface = nullptr;

    if (config.bus_type == BOARD_I2C_BUS) {
        interface = RM3100_I2C_interface(config.bus, config.bus_frequency);

    } else if (config.bus_type == BOARD_SPI_BUS) {
        interface = RM3100_SPI_interface(config.bus, config.spi_devid, config.bus_frequency, config.spi_mode);
    }

    if (interface == nullptr) {
        PX4_ERR("alloc failed");
        return nullptr;
    }

    if (interface->init() != OK) {
        delete interface;
        PX4_DEBUG("no device on bus %i (devid 0x%x)", config.bus, config.spi_devid);
        return nullptr;
    }

    RM3100 *dev = new RM3100(interface, config);

    if (dev == nullptr) {
        delete interface;
        return nullptr;
    }

    if (OK != dev->init()) {
        delete dev;
        return nullptr;
    }

    return dev;
}

void RM3100::print_usage() {
    PRINT_MODULE_USAGE_NAME("rm3100", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("magnetometer");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, true);
    PRINT_MODULE_USAGE_PARAM_INT('R', 0, 0, 35, "Rotation", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int rm3100_main(int argc, char *argv[]) {
    using ThisDriver = RM3100;
    int             ch;
    BusCLIArguments cli{true, true};
    cli.default_i2c_frequency = 400000;
    cli.default_spi_frequency = 1 * 1000 * 1000;

    while ((ch = cli.getOpt(argc, argv, "R:")) != EOF) {
        switch (ch) {
        case 'R':
            cli.rotation = (enum Rotation)atoi(cli.optArg());
            break;
        }
    }

    const char *verb = cli.optArg();

    if (!verb) {
        ThisDriver::print_usage();
        return -1;
    }

    cli.i2c_address = RM3100_ADDRESS;

    BusInstanceIterator iterator(MODULE_NAME, cli, DRV_MAG_DEVTYPE_RM3100);

    if (!strcmp(verb, "start")) {
        return ThisDriver::module_start(cli, iterator);

    } else if (!strcmp(verb, "stop")) {
        return ThisDriver::module_stop(iterator);

    } else if (!strcmp(verb, "status")) {
        return ThisDriver::module_status(iterator);
    }

    ThisDriver::print_usage();
    return -1;
}
