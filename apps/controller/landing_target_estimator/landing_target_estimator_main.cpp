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
 * @file landing_target_estimator_main.cpp
 * Landing target position estimator. Filter and publish the position of a landing target on the ground as observed by an onboard sensor.
 *
 * @author Nicolas de Palezieux (Sunflower Labs) <ndepal@gmail.com>
 * @author Mohammed Kabir <kabir@uasys.io>
 *
 */

#include <defines.h>


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <hrtimer.h>
#include <systemlib/err.h>

#include "LandingTargetEstimator.h"

namespace landing_target_estimator {

static bool thread_should_exit = false; /**< daemon exit flag */
static bool thread_running     = false; /**< daemon status flag */
static int  daemon_task;                /**< Handle of daemon task / thread */

/* Run main loop at this rate in Hz. */
static constexpr uint32_t landing_target_estimator_UPDATE_RATE_HZ = 50;

/**
 * Landing target position estimator app start / stop handling function
 * This makes the module accessible from the nuttx shell
 * @ingroup apps
 */
extern "C" __EXPORT int landing_target_estimator_main(int argc, char *argv[]);

/**
 * Mainloop of daemon.
 */
int landing_target_estimator_thread_main(int argc, char *argv[]);

/**
 * Main entry point for this module
 **/
int landing_target_estimator_main(int argc, char *argv[]) {
    if (argc < 2) {
        goto exiterr;
    }

    if (argc >= 2 && !strcmp(argv[1], "start")) {
        if (thread_running) {
            PX4_INFO("already running");
            /* this is not an error */
            return 0;
        }

        thread_should_exit = false;
        daemon_task        = px4_task_spawn_cmd("landing_target_estimator",
                                                SCHED_DEFAULT,
                                                SCHED_PRIORITY_DEFAULT,
                                                2100,
                                                landing_target_estimator_thread_main,
                                         (argv) ? (char *const *)&argv[2] : nullptr);
        return 0;
    }

    if (!strcmp(argv[1], "stop")) {
        thread_should_exit = true;

        if (!thread_running) {
            PX4_WARN("landing_target_estimator not running");
        }

        return 0;
    }

    if (!strcmp(argv[1], "status")) {
        if (thread_running) {
            PX4_INFO("running");

        } else {
            PX4_INFO("not started");
        }

        return 0;
    }

exiterr:
    PX4_WARN("usage: landing_target_estimator {start|stop|status}");
    return 1;
}

int landing_target_estimator_thread_main(int argc, char *argv[]) {
    PX4_DEBUG("starting");

    thread_running = true;

    LandingTargetEstimator est;

    while (!thread_should_exit) {
        est.update();
        usleep(1000000 / landing_target_estimator_UPDATE_RATE_HZ);
    }

    PX4_DEBUG("exiting");

    thread_running = false;

    return 0;
}

} // namespace landing_target_estimator
