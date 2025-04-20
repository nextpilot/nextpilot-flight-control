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
#include <px4_platform_common/posix.h>

#include <signal.h>
#include <stdint.h>
#include <queue.h>
#include <stdio.h>
#include <semaphore.h>
#include "work_lock.h"

#ifdef CONFIG_SCHED_WORKQUEUE

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
 * Name: work_queue
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
 *   qid    - The work queue ID (index)
 *   work   - The work structure to queue
 *   worker - The worker callback to be invoked.  The callback will invoked
 *            on the worker thread of execution.
 *   arg    - The argument that will be passed to the workder callback when
 *            int is invoked.
 *   delay  - Delay (in clock ticks) from the time queue until the worker
 *            is invoked. Zero means to perform the work immediately.
 *
 * Returned Value:
 *   Zero on success, a negated errno on failure
 *
 ****************************************************************************/

int work_queue(int qid, struct work_s *work, worker_t worker, void *arg, uint32_t delay) {
    struct wqueue_s *wqueue = &g_work[qid];

    //DEBUGASSERT(work != NULL && (unsigned)qid < NWORKERS);

    /* First, initialize the work structure */

    work->worker = worker; /* Work callback */
    work->arg    = arg;    /* Callback argument */
    work->delay  = delay;  /* Delay until work performed */

    /* Now, time-tag that entry and put it in the work queue.  This must be
	 * done with interrupts disabled.  This permits this function to be called
	 * from with task logic or interrupt handlers.
	 */

    work_lock(qid);
    work->qtime = clock_systimer(); /* Time work queued */

    dq_addlast((dq_entry_t *)work, &wqueue->q);
    px4_task_kill(wqueue->pid, SIGCONT); /* Wake up the worker thread */

    work_unlock(qid);
    return PX4_OK;
}

#endif /* CONFIG_SCHED_WORKQUEUE */
