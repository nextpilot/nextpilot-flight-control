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

#include <assert.h>
#include <queue.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: dq_cat
 *
 * Description:
 *   Move the content of queue1 to the end of queue2.
 *
 ****************************************************************************/

void dq_cat(dq_queue_t *queue1, dq_queue_t *queue2) {
    assert(queue1 != NULL && queue2 != NULL);

    /* If queue2 is empty, then just move queue1 to queue2 */

    if (dq_empty(queue2)) {
        dq_move(queue1, queue2);
    }

    /* Do nothing if queue1 is empty */

    else if (!dq_empty(queue1)) {
        /* Attach the head of queue1 to the final entry of queue2 */

        queue2->tail->flink = queue1->head;
        queue1->head->blink = queue2->tail;

        /* The tail of queue1 is the new tail of queue2 */

        queue2->tail = queue1->tail;
        dq_init(queue1);
    }
}
