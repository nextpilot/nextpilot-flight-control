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
 * @file cdevtest_start.cpp
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 * @author Mark Charlebois <mcharleb@gmail.com>
 */
#include "cdevtest_example.h"

#include <px4_platform_common/app.h>

#include <stdio.h>
#include <string.h>

static int daemon_task; /* Handle of deamon task / thread */

//using namespace px4;

extern "C" __EXPORT int cdev_test_main(int argc, char *argv[]);

int cdev_test_main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: cdevtest {start|stop|status}\n");
        return 1;
    }

    if (!strcmp(argv[1], "start")) {
        if (CDevExample::appState.isRunning()) {
            printf("already running\n");
            /* this is not an error */
            return 0;
        }

        daemon_task = px4_task_spawn_cmd("cdevtest",
                                         SCHED_DEFAULT,
                                         SCHED_PRIORITY_MAX - 5,
                                         2000,
                                         PX4_MAIN,
                                         (argv) ? (char *const *)&argv[2] : (char *const *)nullptr);

        return 0;
    }

    if (!strcmp(argv[1], "stop")) {
        CDevExample::appState.requestExit();
        return 0;
    }

    if (!strcmp(argv[1], "status")) {
        if (CDevExample::appState.isRunning()) {
            printf("is running\n");

        } else {
            printf("not started\n");
        }

        return 0;
    }

    printf("usage: cdevtest_main {start|stop|status}\n");
    return 1;
}
