/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "MS4525DO.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

void MS4525DO::print_usage() {
    PRINT_MODULE_USAGE_NAME("ms4525do", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("airspeed_sensor");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
    PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x28);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int ms4525do_main(int argc, char *argv[]) {
    using ThisDriver = MS4525DO;
    BusCLIArguments cli{true, false};
    cli.i2c_address           = I2C_ADDRESS_DEFAULT;
    cli.default_i2c_frequency = I2C_SPEED;

    const char *verb = cli.parseDefaultArguments(argc, argv);

    if (!verb) {
        ThisDriver::print_usage();
        return -1;
    }

    BusInstanceIterator iterator(MODULE_NAME, cli, DRV_DIFF_PRESS_DEVTYPE_MS4525DO);

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
