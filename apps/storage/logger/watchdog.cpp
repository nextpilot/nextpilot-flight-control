/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "watchdog.h"

#include <ulog/log.h>
// #include <px4_platform_common/tasks.h>

#if defined(__PX4_NUTTX) && !defined(CONFIG_SCHED_INSTRUMENTATION)
#   error watchdog support requires CONFIG_SCHED_INSTRUMENTATION
#endif

using namespace time_literals;

namespace nextpilot {
namespace logger {

bool watchdog_update(watchdog_data_t &watchdog_data, bool semaphore_value_saturated) {
#if defined(__PX4_NUTTX) && defined(CONFIG_BUILD_FLAT)

    if (system_load.initialized && watchdog_data.logger_main_task_index >= 0 && watchdog_data.logger_writer_task_index >= 0) {
        const hrt_abstime             now             = hrt_absolute_time();
        const system_load_taskinfo_s &log_writer_task = system_load.tasks[watchdog_data.logger_writer_task_index];

        if (log_writer_task.valid) {
            // Was it already triggered?
            if (watchdog_data.trigger_time != 0) {
                // If so, restore the priority after 1.5s (enough time to flush the buffer and write the perf data)
                if (now > watchdog_data.trigger_time + 1500_ms) {
                    // Restore priorities to ensure the logger threads cannot adversely affect the system
                    sched_param param{};
                    param.sched_priority = watchdog_data.logger_main_priority;

                    if (system_load.tasks[watchdog_data.logger_main_task_index].valid) {
                        sched_setparam(system_load.tasks[watchdog_data.logger_main_task_index].tcb->pid, &param);
                    }

                    param.sched_priority = watchdog_data.log_writer_priority;
                    sched_setparam(log_writer_task.tcb->pid, &param);

                    // Make sure we won't trigger again
                    watchdog_data.logger_main_task_index = -1;
                }

            } else {
                // Trigger the watchdog if the log writer task has been ready to run for a
                // minimum duration and it has not been scheduled during that time.
                // When the writer is waiting for an SD transfer, it is not in ready state, thus a long dropout
                // will not trigger it. The longest period in ready state I measured was around 70ms,
                // after a param change.
                // Additionally we need to check the main thread as well, because if the main thread gets stalled as well
                // while the writer is idle (no active write), it would not trigger.
                // We do that by checking if the scheduling semaphore counter is saturated for a certain duration.
                // No need to lock the tcb access, since we are in IRQ context

                // update the timestamp if it has been scheduled recently
                if (log_writer_task.curr_start_time > watchdog_data.ready_to_run_timestamp) {
                    watchdog_data.ready_to_run_timestamp = log_writer_task.curr_start_time;
                }

                // update the timestamp if not ready to run or if transitioned into ready to run
                uint8_t current_state = log_writer_task.tcb->task_state;

                if (current_state != TSTATE_TASK_READYTORUN
                    || (watchdog_data.last_state != TSTATE_TASK_READYTORUN
                        && current_state == TSTATE_TASK_READYTORUN)) {
                    watchdog_data.ready_to_run_timestamp = now;
                }

                watchdog_data.last_state = current_state;

#   if 0 // for debugging                                                                            \
         // test code that prints the maximum time in ready state.                                   \
         // Note: we are in IRQ context, and thus are strictly speaking not allowed to use PX4_ERR - \
         // we do it anyway since it's only used for debugging.
				static uint64_t max_time = 0;

				if (now - watchdog_data.ready_to_run_timestamp > max_time) {
					max_time = now - watchdog_data.ready_to_run_timestamp;
				}

				static int counter = 0;

				if (++counter > 300) {
					PX4_ERR("max time in ready: %i ms", (int)max_time / 1000);
					counter = 0;
					max_time = 0;
				}

#   endif

                if (!semaphore_value_saturated) {
                    watchdog_data.sem_counter_saturated_start = now;
                }

                if (watchdog_data.manual_watchdog_trigger || now > watchdog_data.sem_counter_saturated_start + 3_s || now > watchdog_data.ready_to_run_timestamp + 1_s) {
                    sched_param param{};

                    // Get the current priorities
                    if (system_load.tasks[watchdog_data.logger_main_task_index].valid) {
                        sched_getparam(system_load.tasks[watchdog_data.logger_main_task_index].tcb->pid, &param);
                        watchdog_data.logger_main_priority = param.sched_priority;
                    }

                    sched_getparam(log_writer_task.tcb->pid, &param);
                    watchdog_data.log_writer_priority = param.sched_priority;

                    // Boost the priority to make sure the logger continues to write to the log.
                    param.sched_priority = SCHED_PRIORITY_LOG_WATCHDOG;

                    if (system_load.tasks[watchdog_data.logger_main_task_index].valid) {
                        sched_setparam(system_load.tasks[watchdog_data.logger_main_task_index].tcb->pid, &param);
                    }

                    sched_setparam(log_writer_task.tcb->pid, &param);

                    watchdog_data.trigger_time = now;
                    return true;
                }
            }

        } else {
            // should never happen
            watchdog_data.logger_main_task_index = -1;
        }
    }

#endif /* __PX4_NUTTX */

    return false;
}

void watchdog_initialize(const pid_t pid_logger_main, const pthread_t writer_thread, watchdog_data_t &watchdog_data) {
#if defined(__PX4_NUTTX) && defined(CONFIG_BUILD_FLAT)

    // The pthread_t ID is equal to the PID on NuttX
    const pthread_t pid_logger_writer = writer_thread;

    sched_lock(); // need to lock the tcb access

    for (int i = 0; i < CONFIG_FS_PROCFS_MAX_TASKS; i++) {
        if (system_load.tasks[i].valid) {
            if (system_load.tasks[i].tcb->pid == pid_logger_writer) {
                watchdog_data.logger_writer_task_index = i;
            }

            if (system_load.tasks[i].tcb->pid == pid_logger_main) {
                watchdog_data.logger_main_task_index = i;
            }
        }
    }

    sched_unlock();

    if (watchdog_data.logger_writer_task_index == -1 || watchdog_data.logger_main_task_index == -1) {
        // If we land here it means the NuttX implementation changed
        // and one of our assumptions is not valid anymore
        PX4_ERR("watchdog init failed");
    }

#endif /* __PX4_NUTTX */
}

} // namespace logger
} // namespace nextpilot
