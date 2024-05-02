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
 * @file usr_hrt.c
 *
 * Userspace High-resolution timer callouts and timekeeping.
 *
 * This can be used with nuttx userspace
 *
 */

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/shutdown.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>

#include <assert.h>
#include <debug.h>

#include <board_config.h>
#include <drivers/drv_hrt.h>
#include <sys/boardctl.h>

static px4_task_t g_usr_hrt_task = -1;

/**
 * Fetch a never-wrapping absolute time value in microseconds from
 * some arbitrary epoch shortly after system start.
 */
hrt_abstime
hrt_absolute_time(void) {
    hrt_abstime abstime = 0;
    boardctl(HRT_ABSOLUTE_TIME, (uintptr_t)&abstime);
    return abstime;
}

/**
 * Store the absolute time in an interrupt-safe fashion
 */
void hrt_store_absolute_time(volatile hrt_abstime *t) {
    irqstate_t flags = px4_enter_critical_section();
    *t               = hrt_absolute_time();
    px4_leave_critical_section(flags);
}

/**
 * Event dispatcher thread
 */
int event_thread(int argc, char *argv[]) {
    struct hrt_call *entry = NULL;

    while (1) {
        /* Wait for hrt tick */
        boardctl(HRT_WAITEVENT, (uintptr_t)&entry);

        /* HRT event received, dispatch */
        if (entry) {
            entry->usr_callout(entry->usr_arg);
        }
    }

    return 0;
}

/**
 * Request stop.
 */
bool hrt_request_stop() {
    px4_task_delete(g_usr_hrt_task);
    return true;
}

/**
 * Initialise the high-resolution timing module.
 */
void hrt_init(void) {
    px4_register_shutdown_hook(hrt_request_stop);
    g_usr_hrt_task = px4_task_spawn_cmd("usr_hrt", SCHED_DEFAULT, SCHED_PRIORITY_MAX, 1000, event_thread, NULL);
}

/**
 * Call callout(arg) after interval has elapsed.
 */
void hrt_call_after(struct hrt_call *entry, hrt_abstime delay, hrt_callout callout, void *arg) {
    hrt_boardctl_t ioc_parm;
    ioc_parm.entry     = entry;
    ioc_parm.time      = delay;
    ioc_parm.callout   = callout;
    ioc_parm.arg       = arg;
    entry->usr_callout = callout;
    entry->usr_arg     = arg;

    boardctl(HRT_CALL_AFTER, (uintptr_t)&ioc_parm);
}

/**
 * Call callout(arg) at calltime.
 */
void hrt_call_at(struct hrt_call *entry, hrt_abstime calltime, hrt_callout callout, void *arg) {
    hrt_boardctl_t ioc_parm;
    ioc_parm.entry     = entry;
    ioc_parm.time      = calltime;
    ioc_parm.interval  = 0;
    ioc_parm.callout   = callout;
    ioc_parm.arg       = arg;
    entry->usr_callout = callout;
    entry->usr_arg     = arg;

    boardctl(HRT_CALL_AT, (uintptr_t)&ioc_parm);
}

/**
 * Call callout(arg) every period.
 */
void hrt_call_every(struct hrt_call *entry, hrt_abstime delay, hrt_abstime interval, hrt_callout callout, void *arg) {
    hrt_boardctl_t ioc_parm;
    ioc_parm.entry     = entry;
    ioc_parm.time      = delay;
    ioc_parm.interval  = interval;
    ioc_parm.callout   = callout;
    ioc_parm.arg       = arg;
    entry->usr_callout = callout;
    entry->usr_arg     = arg;

    boardctl(HRT_CALL_EVERY, (uintptr_t)&ioc_parm);
}

/**
 * Remove the entry from the callout list.
 */
void hrt_cancel(struct hrt_call *entry) {
    boardctl(HRT_CANCEL, (uintptr_t)entry);
}

void hrt_call_init(struct hrt_call *entry) {
    memset(entry, 0, sizeof(*entry));
}

/**
 * If this returns true, the call has been invoked and removed from the callout list.
 *
 * Always returns false for repeating callouts.
 */
bool hrt_called(struct hrt_call *entry) {
    return (entry->deadline == 0);
}

latency_info_t
get_latency(uint16_t bucket_idx, uint16_t counter_idx) {
    latency_boardctl_t latency_ioc;
    latency_ioc.bucket_idx  = bucket_idx;
    latency_ioc.counter_idx = counter_idx;
    latency_ioc.latency     = {0, 0};
    boardctl(HRT_GET_LATENCY, (uintptr_t)&latency_ioc);
    return latency_ioc.latency;
}

void reset_latency_counters() {
    boardctl(HRT_RESET_LATENCY, NULL);
}
