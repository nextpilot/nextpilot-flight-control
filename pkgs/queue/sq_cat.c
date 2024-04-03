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
 * Name: sq_cat
 *
 * Description:
 *   Move the content of queue1 to the end of queue2.
 *
 ****************************************************************************/

void sq_cat(sq_queue_t *queue1, sq_queue_t *queue2) {
    assert(queue1 != NULL && queue2 != NULL);

    /* If queue2 is empty, then just move queue1 to queue2 */

    if (sq_empty(queue2)) {
        sq_move(queue1, queue2);
    }

    /* Do nothing if queue1 is empty */

    else if (!sq_empty(queue1)) {
        /* Attach the head of queue1 to the final entry of queue2 */

        queue2->tail->flink = queue1->head;

        /* The tail of queue1 is the new tail of queue2 */

        queue2->tail = queue1->tail;
        sq_init(queue1);
    }
}
