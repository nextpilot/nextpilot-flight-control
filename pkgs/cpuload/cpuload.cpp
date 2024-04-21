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
 * @file cpuload.c
 *
 * Measurement of CPU load of each individual task.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Petri Tanskanen <petri.tanskanen@inf.ethz.ch>
 */

#include <atomic/atomic.hpp>
#include <cpuload.h>
#include <hrtimer.h>

#if defined(__PX4_NUTTX) && defined(CONFIG_SCHED_INSTRUMENTATION)
__BEGIN_DECLS
#   include <nuttx/sched_note.h>

__EXPORT struct system_load_s system_load;

#   if defined(CONFIG_SEGGER_SYSVIEW)
#      include <nuttx/note/note_sysview.h>
#      ifndef CONFIG_SEGGER_SYSVIEW_PREFIX
#         error Systemview enabled but prefix is not
#      endif
#   endif

static atomic_int cpuload_monitor_all_count{0};

void cpuload_monitor_start() {
    if (cpuload_monitor_all_count.fetch_add(1) == 0) {
        // if the count was previously 0 (idle thread only) then clear any existing runtime data
        sched_lock();

        system_load.start_time = hrt_absolute_time();

        for (int i = 1; i < CONFIG_FS_PROCFS_MAX_TASKS; i++) {
            system_load.tasks[i].total_runtime   = 0;
            system_load.tasks[i].curr_start_time = 0;
        }

        sched_unlock();
    }
}

void cpuload_monitor_stop() {
    if (cpuload_monitor_all_count.fetch_sub(1) <= 1) {
        // don't allow the count to go negative
        cpuload_monitor_all_count.store(0);
    }
}

void cpuload_initialize_once() {
    for (auto &task : system_load.tasks) {
        task.valid = false;
    }

    int static_tasks_count = 2; // there are at least 2 threads that should be initialized statically - "idle" and "init"

#   ifdef CONFIG_PAGING
    static_tasks_count++; // include paging thread in initialization
#   endif                 /* CONFIG_PAGING */
#   if CONFIG_SCHED_WORKQUEUE
    static_tasks_count++; // include high priority work0 thread in initialization
#   endif                 /* CONFIG_SCHED_WORKQUEUE */
#   if CONFIG_SCHED_LPWORK
    static_tasks_count++; // include low priority work1 thread in initialization
#   endif                 /* CONFIG_SCHED_WORKQUEUE */

    // perform static initialization of "system" threads
    for (system_load.total_count = 0; system_load.total_count < static_tasks_count; system_load.total_count++) {
        system_load.tasks[system_load.total_count].total_runtime   = 0;
        system_load.tasks[system_load.total_count].curr_start_time = 0;
        system_load.tasks[system_load.total_count].tcb             = nxsched_get_tcb(
            system_load.total_count); // it is assumed that these static threads have consecutive PIDs
        system_load.tasks[system_load.total_count].valid = true;
    }

    system_load.initialized = true;
}

void sched_note_start(FAR struct tcb_s *tcb) {
    // find first free slot
    if (system_load.initialized) {
        for (auto &task : system_load.tasks) {
            if (!task.valid) {
                // slot is available
                task.total_runtime   = 0;
                task.curr_start_time = 0;
                task.tcb             = tcb;
                task.valid           = true;
                system_load.total_count++;
                break;
            }
        }
    }

#   ifdef CONFIG_SEGGER_SYSVIEW
    sysview_sched_note_start(tcb);
#   endif
}

void sched_note_stop(FAR struct tcb_s *tcb) {
    if (system_load.initialized) {
        for (auto &task : system_load.tasks) {
            if (task.tcb && task.tcb->pid == tcb->pid) {
                // mark slot as free
                task.valid           = false;
                task.total_runtime   = 0;
                task.curr_start_time = 0;
                task.tcb             = nullptr;
                system_load.total_count--;
                break;
            }
        }
    }

#   ifdef CONFIG_SEGGER_SYSVIEW
    sysview_sched_note_stop(tcb);
#   endif
}

void sched_note_suspend(FAR struct tcb_s *tcb) {
    if (system_load.initialized) {
        if (tcb->pid == 0) {
            system_load.tasks[0].total_runtime += hrt_elapsed_time(&system_load.tasks[0].curr_start_time);
            return;

        } else {
            if (cpuload_monitor_all_count.load() == 0) {
                return;
            }
        }

        for (auto &task : system_load.tasks) {
            // Task ending its current scheduling run
            if (task.valid && (task.curr_start_time > 0) && task.tcb && task.tcb->pid == tcb->pid) {
                task.total_runtime += hrt_elapsed_time(&task.curr_start_time);
                break;
            }
        }
    }

#   ifdef CONFIG_SEGGER_SYSVIEW
    sysview_sched_note_suspend(tcb);
#   endif
}

void sched_note_resume(FAR struct tcb_s *tcb) {
    if (system_load.initialized) {
        if (tcb->pid == 0) {
            hrt_store_absolute_time(&system_load.tasks[0].curr_start_time);
            return;

        } else {
            if (cpuload_monitor_all_count.load() == 0) {
                return;
            }
        }

        for (auto &task : system_load.tasks) {
            if (task.valid && task.tcb && task.tcb->pid == tcb->pid) {
                // curr_start_time is accessed from an IRQ handler (in logger), so we need
                // to make the update atomic
                hrt_store_absolute_time(&task.curr_start_time);
                break;
            }
        }
    }

#   ifdef CONFIG_SEGGER_SYSVIEW
    sysview_sched_note_resume(tcb);
#   endif
}

#   ifdef CONFIG_SEGGER_SYSVIEW

#      ifdef CONFIG_SCHED_INSTRUMENTATION_IRQHANDLER
void sched_note_irqhandler(int irq, FAR void *handler, bool enter) {
    sysview_sched_note_irqhandler(irq, handler, enter);
}
#      endif

#      ifdef CONFIG_SCHED_INSTRUMENTATION_SYSCALL
void sched_note_syscall_enter(int nr);
{
    sysview_sched_note_syscall_enter(nr);
}

void sched_note_syscall_enter(int nr);
{
    sysview_sched_note_syscall_enter(nr);
}
#      endif

#   endif

__END_DECLS
#endif // PX4_NUTTX && CONFIG_SCHED_INSTRUMENTATION
