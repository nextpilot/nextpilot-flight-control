#ifndef __NORMLIST_H__
#define __NORMLIST_H__

// 单向链表节点
typedef struct sq_entry_s {
    union {
        struct sq_entry_s *next;
        struct sq_entry_s *flink;
    };
} sq_entry_t;

// 单向链表
typedef struct sq_queue_s {
    struct sq_entry_s *head;
    struct sq_entry_s *tail;
} sq_queue_t;

// 双向链表节点
typedef struct dq_entry_s {
    union {
        struct dq_entry_s *blink;
        struct dq_entry_s *prev;
    };
    union {
        struct dq_entry_s *flink;
        struct dq_entry_s *next;
    };
} dq_entry_t;

// 双向链表
typedef struct sq_queue_s {
    struct dq_entry_s *head;
    struct dq_entry_s *tail;
} dq_queue_t;

#define sq_list_init(q) (q->next = NULL)

size_t sq_count(sq_queue_t *q) {
    size_t count = 0;
    for (sq_entry_t *node = q->head, count = 0;
         node != NULL           /*普通链表*/
         && node != q->head     /*环形链表*/
         && node != node->next; /*防止循环*/
         node = node->next, count++) {};
    return count;
}

void sq_addfirst(sq_entry_t *node, sq_queue_t *queue) {
    // 如果节点指向自己的表示环形节点，如果节点指向NULL是普通节点
    assert(node != NULL || (node->next != NULL /*普通链表*/ && node->next != node /*环形链表*/));
    assert(queue != NULL);

    if (!queue->head) {
        queue->tail = node;
    } else {
        node->next = queue->head;
    }
    queue->head = node;
}

void sq_addlast() {
}

void sq_addbefore() {
}

void sq_addafter(sq_entry_t *prev, sq_entry_t *node, sq_queue_t *queue) {
    assert(prev != NULL && (prev->next == NULL || prev->next == prev));
    assert(node != NULL && (node->next != NULL && node->next != node));

    if (queue) {
    } else {
    }
}

void sq_rem(sq_entry_t *node, sq_queue_t *queue) {
    assert(node != NULL && node->next != node);

    if (queue) { // 普通链表
        if (queue->head) {
            if (node == queue->head) {
            }
        }
    } else { // 环形链表
    }
}

void sq_remfirst() {
}

void sq_remlast() {
}

void sq_cat() {
}

#endif // __NORMLIST_H__