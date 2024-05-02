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

#if defined(__PX4_NUTTX)
#   include <nuttx/arch.h>
#   include <nuttx/wqueue.h>
#   include <nuttx/clock.h>
#elif defined(__PX4_POSIX)

#   include <stdint.h>
#   include <queue.h>
#   include <sys/types.h>

__BEGIN_DECLS

#   define HPWORK   0
#   define LPWORK   1
#   define NWORKERS 2

struct wqueue_s {
    pid_t             pid; /* The task ID of the worker thread */
    struct dq_queue_s q;   /* The queue of pending work */
};

extern struct wqueue_s g_work[NWORKERS];

/* Defines the work callback */

typedef void (*worker_t)(void *arg);

struct work_s {
    struct dq_entry_s dq;     /* Implements a doubly linked list */
    worker_t          worker; /* Work callback */
    void             *arg;    /* Callback argument */
    uint64_t          qtime;  /* Time work queued */
    uint32_t          delay;  /* Delay until work performed */
};

/****************************************************************************
 * Name: work_queues_init()
 *
 * Description:
 *   Initialize the work queues.
 *
 ****************************************************************************/
void work_queues_init(void);

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
 *   qid    - The work queue ID
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

int work_queue(int qid, struct work_s *work, worker_t worker, void *arg, uint32_t delay);

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

int work_cancel(int qid, struct work_s *work);

uint32_t clock_systimer(void);

int work_hpthread(int argc, char *argv[]);
int work_lpthread(int argc, char *argv[]);

__END_DECLS

#else
#   error "Unknown target OS"
#endif
