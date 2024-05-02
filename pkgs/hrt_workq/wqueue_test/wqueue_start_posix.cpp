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
 * @file wqueue_start_posix.cpp
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 * @author Mark Charlebois <mcharleb@gmail.com>
 */
#include "wqueue_test.h"

#include <px4_platform_common/app.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/tasks.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

static int daemon_task; /* Handle of deamon task / thread */

//using namespace px4;

extern "C" __EXPORT int wqueue_test_main(int argc, char *argv[]);

int wqueue_test_main(int argc, char *argv[]) {
    if (argc < 2) {
        PX4_INFO("usage: wqueue_test {start|stop|status}\n");
        return 1;
    }

    if (!strcmp(argv[1], "start")) {
        if (WQueueTest::appState.isRunning()) {
            PX4_INFO("already running\n");
            /* this is not an error */
            return 0;
        }

        daemon_task = px4_task_spawn_cmd("wqueue",
                                         SCHED_DEFAULT,
                                         SCHED_PRIORITY_MAX - 5,
                                         2000,
                                         PX4_MAIN,
                                         (argv) ? (char *const *)&argv[2] : (char *const *)nullptr);

        return 0;
    }

    if (!strcmp(argv[1], "stop")) {
        WQueueTest::appState.requestExit();
        return 0;
    }

    if (!strcmp(argv[1], "status")) {
        if (WQueueTest::appState.isRunning()) {
            PX4_INFO("is running\n");

        } else {
            PX4_INFO("not started\n");
        }

        return 0;
    }

    PX4_INFO("usage: wqueue_test {start|stop|status}\n");
    return 1;
}
