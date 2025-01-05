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
#include <module/module_usage.h>

#include "BMI088.hpp"

int BMI088::print_usage(const char *reason) {
    PRINT_MODULE_USAGE_NAME("bmi088", "driver");
    PRINT_MODULE_USAGE_SUBCATEGORY("imu");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_PARAM_FLAG('A', "Accel", true);
    PRINT_MODULE_USAGE_PARAM_FLAG('G', "Gyro", true);
    PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(false, true);
    PRINT_MODULE_USAGE_PARAM_INT('R', 0, 0, 35, "Rotation", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

static int bmi088_main(int argc, char *argv[]) {
    return BMI088::main(argc, argv);
}

int bmi088_start() {
    uint32_t sys_hitl = param_get_int32((param_t)params_id::SYS_HITL);

    if (sys_hitl != 0) {
        return 0;
    }

    int ret = 0;

    // start accel
    {
        const char *argv[]  = {"bmi088", "start", "-A", "-R", STRINGIFY(2), "-d", "bmi088_accel"};
        int         argc    = sizeof(argv) / sizeof(argv[0]);
        ret                += BMI088::main(argc, (char **)argv);
    }
    // start gyro
    {
        const char *argv[]  = {"bmi088", "start", "-G", "-R", STRINGIFY(BMI088_ROTATION), "-d", "bmi088_gyro"};
        int         argc    = sizeof(argv) / sizeof(argv[0]);
        ret                += BMI088::main(argc, (char **)argv);
    }

    return ret;
}
