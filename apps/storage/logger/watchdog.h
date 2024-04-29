/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <hrtimer.h>
#include <pthread.h>

#ifdef __PX4_NUTTX
#   include <nuttx/sched.h>
#   include <px4_platform/cpuload.h>
#endif /* __PX4_NUTTX */

namespace nextpilot ::logger {

struct watchdog_data_t {
#ifdef __PX4_NUTTX
    int         logger_main_task_index      = -1;
    int         logger_writer_task_index    = -1;
    hrt_abstime ready_to_run_timestamp      = hrt_absolute_time();
    hrt_abstime sem_counter_saturated_start = hrt_absolute_time();
    uint8_t     last_state                  = TSTATE_TASK_INVALID;
    int         log_writer_priority         = 0;
    int         logger_main_priority        = 0;
    hrt_abstime trigger_time                = 0; ///< timestamp when it was triggered
    bool        manual_watchdog_trigger     = false;
#endif                                           /* __PX4_NUTTX */
};

/**
 * Initialize the watchdog, fill in watchdog_data.
 */
void watchdog_initialize(const pid_t pid_logger_main, const pthread_t writer_thread, watchdog_data_t &watchdog_data);

/**
 * Update the watchdog and trigger it if necessary. It is triggered when the log writer task is in
 * ready state for a certain period of time, but did not get scheduled. It means that most likely
 * some other higher-prio task runs busy.
 * When the watchdog triggers, it boosts the priority of the logger's main & writer tasks to maximum, so
 * that they get scheduled again.
 *
 * Expected to be called from IRQ context.
 *
 * @param watchdog_data
 * @param semaphore_value_saturated if the scheduling semaphore counter is saturated
 * @return true if watchdog is triggered, false otherwise
 */
bool watchdog_update(watchdog_data_t &watchdog_data, bool semaphore_value_saturated);

} // namespace nextpilot::logger
