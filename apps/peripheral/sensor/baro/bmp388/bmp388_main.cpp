/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

#include "bmp388.h"

void BMP388::print_usage() {
    PRINT_MODULE_USAGE_NAME("bmp388", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("baro");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, true);
    PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x76);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

I2CSPIDriverBase *BMP388::instantiate(const I2CSPIDriverConfig &config, int runtime_instance) {
    IBMP388 *interface = nullptr;

    if (config.bus_type == BOARD_I2C_BUS) {
        interface = bmp388_i2c_interface(config.bus, config.i2c_address, config.bus_frequency);

    } else if (config.bus_type == BOARD_SPI_BUS) {
        interface = bmp388_spi_interface(config.bus, config.spi_devid, config.bus_frequency, config.spi_mode);
    }

    if (interface == nullptr) {
        LOG_E("failed creating interface for bus %i (devid 0x%" PRIx32 ")", config.bus, config.spi_devid);
        return nullptr;
    }

    if (interface->init() != OK) {
        delete interface;
        LOG_D("no device on bus %i (devid 0x%" PRIx32 ")", config.bus, config.spi_devid);
        return nullptr;
    }

    BMP388 *dev = new BMP388(config, interface);

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

extern "C" int bmp388_main(int argc, char *argv[]) {
    using ThisDriver = BMP388;
    BusCLIArguments cli{true, true};
    cli.i2c_address           = 0x76;
    cli.default_i2c_frequency = 100 * 1000;
    cli.default_spi_frequency = 10 * 1000 * 1000;

    const char *verb = cli.parseDefaultArguments(argc, argv);

    if (!verb) {
        ThisDriver::print_usage();
        return -1;
    }

    BusInstanceIterator iterator(MODULE_NAME, cli, DRV_BARO_DEVTYPE_BMP388);

    if (!strcmp(verb, "start")) {
        return ThisDriver::module_start(cli, iterator);
    }

    if (!strcmp(verb, "stop")) {
        return ThisDriver::module_stop(iterator);
    }

    if (!strcmp(verb, "status")) {
        return ThisDriver::module_status(iterator);
    }

    ThisDriver::print_usage();
    return -1;
}
