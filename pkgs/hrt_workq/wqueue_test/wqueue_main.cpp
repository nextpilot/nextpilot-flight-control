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
 * @file wqueue_main.cpp
 * Example for Linux
 *
 * @author Mark Charlebois <charlebm@gmail.com>
 */
#include "wqueue_test.h"

#include <px4_platform_common/log.h>
#include <px4_platform_common/app.h>
#include <stdio.h>

int PX4_MAIN(int argc, char **argv) {
    px4::init(argc, argv, "wqueue_test");

    PX4_INFO("wqueue hello\n");
    WQueueTest wq;
    wq.main();

    PX4_INFO("goodbye\n");
    return 0;
}
