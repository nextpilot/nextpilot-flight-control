/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ICM42688P.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>

void ICM42688P::print_usage() {
    PRINT_MODULE_USAGE_NAME("icm42688p", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("imu");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(false, true);
    PRINT_MODULE_USAGE_PARAM_INT('R', 0, 0, 35, "Rotation", true);
    PRINT_MODULE_USAGE_PARAM_INT('C', 0, 0, 35000, "Input clock frequency (Hz)", true);
    PRINT_MODULE_USAGE_PARAM_FLAG('6', "Drive ICM-42686", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int icm42688p_main(int argc, char *argv[]) {
    int ch;
    using ThisDriver = ICM42688P;
    BusCLIArguments cli{false, true};
    cli.default_spi_frequency = SPI_SPEED;

    while ((ch = cli.getOpt(argc, argv, "C:R:6")) != EOF) {
        switch (ch) {
        case 'C':
            cli.custom1 = atoi(cli.optArg());
            break;

        case 'R':
            cli.rotation = (enum Rotation)atoi(cli.optArg());
            break;

        case '6':
            cli.custom2 = DRV_IMU_DEVTYPE_ICM42686P;
            break;
        }
    }

    const char *verb = cli.optArg();

    if (!verb) {
        ThisDriver::print_usage();
        return -1;
    }

    BusInstanceIterator iterator(cli.custom2 == DRV_IMU_DEVTYPE_ICM42686P ? "icm42686p" : MODULE_NAME, cli,
                                 cli.custom2 == DRV_IMU_DEVTYPE_ICM42686P ? DRV_IMU_DEVTYPE_ICM42686P : DRV_IMU_DEVTYPE_ICM42688P);

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
