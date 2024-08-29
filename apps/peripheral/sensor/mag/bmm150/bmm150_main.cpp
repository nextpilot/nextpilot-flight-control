/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "BMM150.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

void BMM150::print_usage() {
    PRINT_MODULE_USAGE_NAME("bmm150", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("magnetometer");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
    PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x10);
    PRINT_MODULE_USAGE_PARAM_INT('R', 0, 0, 35, "Rotation", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int bmm150_main(int argc, char *argv[]) {
    int ch;
    using ThisDriver = BMM150;
    BusCLIArguments cli{true, false};
    cli.default_i2c_frequency = I2C_SPEED;
    cli.i2c_address           = I2C_ADDRESS_DEFAULT;

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

    BusInstanceIterator iterator(MODULE_NAME, cli, DRV_MAG_DEVTYPE_BMM150);

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
