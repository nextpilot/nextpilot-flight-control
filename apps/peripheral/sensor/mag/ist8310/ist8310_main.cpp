/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "IST8310.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

void IST8310::print_usage() {
    PRINT_MODULE_USAGE_NAME("ist8310", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("magnetometer");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
    PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x0E);
    PRINT_MODULE_USAGE_PARAM_INT('R', 0, 0, 35, "Rotation", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int ist8310_main(int argc, char *argv[]) {
    int ch;
    using ThisDriver = IST8310;
    BusCLIArguments cli{true, false};
    cli.i2c_address           = I2C_ADDRESS_DEFAULT;
    cli.default_i2c_frequency = I2C_SPEED;

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

    BusInstanceIterator iterator(MODULE_NAME, cli, DRV_MAG_DEVTYPE_IST8310);

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
