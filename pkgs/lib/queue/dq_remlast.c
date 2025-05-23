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

#include <queue.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/***************************************************(************************
 * Name: dq_remlast
 *
 * Description:
 *   dq_remlast removes the last entry from 'queue'
 *
 ****************************************************************************/

dq_entry_t *dq_remlast(dq_queue_t *queue) {
    dq_entry_t *ret = queue->tail;

    if (ret) {
        dq_entry_t *prev = ret->blink;
        if (!prev) {
            queue->head = NULL;
            queue->tail = NULL;
        } else {
            queue->tail = prev;
            prev->flink = NULL;
        }

        ret->flink = NULL;
        ret->blink = NULL;
    }

    return ret;
}
