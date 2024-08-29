/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "PAW3902.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

void PAW3902::print_usage() {
    PRINT_MODULE_USAGE_NAME("paw3902", "driver");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(false, true);
    PRINT_MODULE_USAGE_PARAM_INT('Y', 0, 0, 359, "custom yaw rotation (degrees)", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" __EXPORT int paw3902_main(int argc, char *argv[]) {
    int ch           = 0;
    using ThisDriver = PAW3902;
    BusCLIArguments cli{false, true};
    cli.custom1               = -1;
    cli.spi_mode              = SPIDEV_MODE3;
    cli.default_spi_frequency = SPI_SPEED;

    while ((ch = cli.getOpt(argc, argv, "Y:")) != EOF) {
        switch (ch) {
        case 'Y':
            cli.custom1 = atoi(cli.optArg());
            break;
        }
    }

    const char *verb = cli.optArg();

    if (!verb) {
        ThisDriver::print_usage();
        return -1;
    }

    BusInstanceIterator iterator(MODULE_NAME, cli, DRV_FLOW_DEVTYPE_PAW3902);

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
