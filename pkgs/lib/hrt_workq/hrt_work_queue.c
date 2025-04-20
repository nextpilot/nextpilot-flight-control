/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/workqueue.h>
#include <px4_platform_common/tasks.h>

#include <signal.h>
#include <stdint.h>
#include <queue.h>
#include <stdio.h>
#include <semaphore.h>
#include <drivers/drv_hrt.h>
#include <px4_platform_common/workqueue.h>
#include <px4_platform_common/posix.h>
#include "hrt_work.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Public Variables
 ****************************************************************************/

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: hrt_work_queue
 *
 * Description:
 *   Queue work to be performed at a later time.  All queued work will be
 *   performed on the worker thread of of execution (not the caller's).
 *
 *   The work structure is allocated by caller, but completely managed by
 *   the work queue logic.  The caller should never modify the contents of
 *   the work queue structure; the caller should not call work_queue()
 *   again until either (1) the previous work has been performed and removed
 *   from the queue, or (2) work_cancel() has been called to cancel the work
 *   and remove it from the work queue.
 *
 * Input parameters:
 *   work   - The work structure to queue
 *   worker - The worker callback to be invoked.  The callback will invoked
 *            on the worker thread of execution.
 *   arg    - The argument that will be passed to the workder callback when
 *            int is invoked.
 *   delay  - Delay (in microseconds) from the time queue until the worker
 *            is invoked. Zero means to perform the work immediately.
 *
 * Returned Value:
 *   Zero on success, a negated errno on failure
 *
 ****************************************************************************/

int hrt_work_queue(struct work_s *work, worker_t worker, void *arg, uint32_t delay) {
    struct wqueue_s *wqueue = &g_hrt_work;

    /* First, initialize the work structure */

    work->worker = worker; /* Work callback */
    work->arg    = arg;    /* Callback argument */
    work->delay  = delay;  /* Delay until work performed */

    /* Now, time-tag that entry and put it in the work queue.  This must be
	 * done with interrupts disabled.  This permits this function to be called
	 * from with task logic or interrupt handlers.
	 */

    hrt_work_lock();
    work->qtime = hrt_absolute_time(); /* Time work queued */
    //PX4_INFO("hrt work_queue adding work delay=%u time=%lu", delay, work->qtime);

    dq_addlast(&work->dq, &wqueue->q);

    if (px4_getpid() != wqueue->pid) { /* only need to wake up if called from a different thread */
        //wqueue->pid == own task? -> don't signal
        px4_task_kill(wqueue->pid, SIGCONT); /* Wake up the worker thread */
    }

    hrt_work_unlock();
    return PX4_OK;
}
