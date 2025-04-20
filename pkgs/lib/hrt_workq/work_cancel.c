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
#include <queue.h>
#include <px4_platform_common/workqueue.h>
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
 * Name: work_cancel
 *
 * Description:
 *   Cancel previously queued work.  This removes work from the work queue.
 *   After work has been canceled, it may be re-queue by calling work_queue()
 *   again.
 *
 * Input parameters:
 *   qid    - The work queue ID
 *   work   - The previously queue work structure to cancel
 *
 * Returned Value:
 *   Zero on success, a negated errno on failure
 *
 ****************************************************************************/

int work_cancel(int qid, struct work_s *work) {
    struct wqueue_s *wqueue = &g_work[qid];

    //DEBUGASSERT(work != NULL && (unsigned)qid < NWORKERS);

    /* Cancelling the work is simply a matter of removing the work structure
	 * from the work queue.  This must be done with interrupts disabled because
	 * new work is typically added to the work queue from interrupt handlers.
	 */

    work_lock(qid);

    if (work->worker != NULL) {
        /* A little test of the integrity of the work queue */

        //DEBUGASSERT(work->dq.flink ||(dq_entry_t *)work == wqueue->q.tail);
        //DEBUGASSERT(work->dq.blink ||(dq_entry_t *)work == wqueue->q.head);

        /* Remove the entry from the work queue and make sure that it is
		 * mark as availalbe (i.e., the worker field is nullified).
		 */

        dq_rem((dq_entry_t *)work, &wqueue->q);
        work->worker = NULL;
    }

    work_unlock(qid);
    return PX4_OK;
}

#endif /* CONFIG_SCHED_WORKQUEUE */
