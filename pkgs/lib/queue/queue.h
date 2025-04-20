/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __INCLUDE_QUEUE_H
#define __INCLUDE_QUEUE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sys/types.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define sq_init(q)        \
    do {                  \
        (q)->head = NULL; \
        (q)->tail = NULL; \
    } while (0)

#define dq_init(q)        \
    do {                  \
        (q)->head = NULL; \
        (q)->tail = NULL; \
    } while (0)

#define sq_move(q1, q2)          \
    do {                         \
        (q2)->head = (q1)->head; \
        (q2)->tail = (q1)->tail; \
        (q1)->head = NULL;       \
        (q1)->tail = NULL;       \
    } while (0)

#define dq_move(q1, q2)          \
    do {                         \
        (q2)->head = (q1)->head; \
        (q2)->tail = (q1)->tail; \
        (q1)->head = NULL;       \
        (q1)->tail = NULL;       \
    } while (0)

#define sq_next(p) ((p)->flink)
#define dq_next(p) ((p)->flink)
#define dq_prev(p) ((p)->blink)

#define sq_empty(q) ((q)->head == NULL)
#define dq_empty(q) ((q)->head == NULL)

#define sq_peek(q) ((q)->head)
#define dq_peek(q) ((q)->head)

#define sq_tail(q) ((q)->tail)
#define dq_tail(q) ((q)->tail)

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

struct sq_entry_s {
    struct sq_entry_s *flink;
};
typedef struct sq_entry_s sq_entry_t;

struct dq_entry_s {
    struct dq_entry_s *flink;
    struct dq_entry_s *blink;
};
typedef struct dq_entry_s dq_entry_t;

struct sq_queue_s {
    sq_entry_t *head;
    sq_entry_t *tail;
};
typedef struct sq_queue_s sq_queue_t;

struct dq_queue_s {
    dq_entry_t *head;
    dq_entry_t *tail;
};
typedef struct dq_queue_s dq_queue_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/* Add nodes to queues */

void sq_addfirst(sq_entry_t *node, sq_queue_t *queue);
void dq_addfirst(dq_entry_t *node, dq_queue_t *queue);
void sq_addlast(sq_entry_t *node, sq_queue_t *queue);
void dq_addlast(dq_entry_t *node, dq_queue_t *queue);
void sq_addafter(sq_entry_t *prev, sq_entry_t *node,
                 sq_queue_t *queue);
void dq_addafter(dq_entry_t *prev, dq_entry_t *node,
                 dq_queue_t *queue);
void dq_addbefore(dq_entry_t *next, dq_entry_t *node,
                  dq_queue_t *queue);

/* Combine queues */

void sq_cat(sq_queue_t *queue1, sq_queue_t *queue2);
void dq_cat(dq_queue_t *queue1, dq_queue_t *queue2);

/* Remove nodes from queues */

sq_entry_t *sq_remafter(sq_entry_t *node, sq_queue_t *queue);
void        sq_rem(sq_entry_t *node, sq_queue_t *queue);
void        dq_rem(dq_entry_t *node, dq_queue_t *queue);
sq_entry_t *sq_remlast(sq_queue_t *queue);
dq_entry_t *dq_remlast(dq_queue_t *queue);
sq_entry_t *sq_remfirst(sq_queue_t *queue);
dq_entry_t *dq_remfirst(dq_queue_t *queue);

/* Count nodes in queues */

size_t sq_count(sq_queue_t *queue);
size_t dq_count(dq_queue_t *queue);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_QUEUE_H_ */
