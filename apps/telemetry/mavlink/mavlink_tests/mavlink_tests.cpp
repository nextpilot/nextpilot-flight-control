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
 * @file mavlink_ftp_tests.cpp
 */

#include <systemlib/err.h>

#include "mavlink_ftp_test.h"

extern "C" __EXPORT int mavlink_tests_main(int argc, char *argv[]);

int mavlink_tests_main(int argc, char *argv[]) {
    return mavlink_ftp_test() ? 0 : -1;
}
