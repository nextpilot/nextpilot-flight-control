/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include <getopt/getopt.h>
#include <module/module_params.h>
#include <module/module_command.h>

#include "BMI055.hpp"

void BMI055::print_usage() {
    PRINT_MODULE_USAGE_NAME("bmi055", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("imu");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAM_FLAG('A', "Accel", true);
    PRINT_MODULE_USAGE_PARAM_FLAG('G', "Gyro", true);
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(false, true);
    PRINT_MODULE_USAGE_PARAM_INT('R', 0, 0, 35, "Rotation", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" int bmi055_main(int argc, char *argv[]) {
    int ch;
    using ThisDriver = BMI055;
    BusCLIArguments cli{false, true};
    uint16_t        type      = 0;
    cli.default_spi_frequency = 10000000;
    const char *name          = MODULE_NAME;

    while ((ch = cli.getOpt(argc, argv, "AGR:")) != EOF) {
        switch (ch) {
        case 'A':
            type = DRV_ACC_DEVTYPE_BMI055;
            name = MODULE_NAME "_accel";
            break;

        case 'G':
            type = DRV_GYR_DEVTYPE_BMI055;
            name = MODULE_NAME "_gyro";
            break;

        case 'R':
            cli.rotation = (enum Rotation)atoi(cli.optArg());
            break;
        }
    }

    const char *verb = cli.optArg();

    if (!verb || type == 0) {
        ThisDriver::print_usage();
        return -1;
    }

    BusInstanceIterator iterator(name, cli, type);

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
