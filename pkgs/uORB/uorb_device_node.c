
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "uorb_device_node.h"
#include <rtdbg.h>

// #define ORB_MULTI_MAX_INSTANCES 4
#define ORB_TOPICS_COUNT  10

#define ORB_BITS_PER_ELEM (32)
#define ORB_TOTAL_BITS    (ORB_TOPICS_COUNT * ORB_MULTI_MAX_INSTANCES)
#define ORB_TOTAL_ELEM    ((ORB_TOTAL_BITS % ORB_BITS_PER_ELEM == 0) ? (ORB_TOTAL_BITS / ORB_BITS_PER_ELEM) : (ORB_TOTAL_BITS / ORB_BITS_PER_ELEM + 1))

typedef uint16_t ORB_ID;

static rt_list_t _uorb_device_list                  = {NULL, NULL};
static uint32_t  _uorb_device_exist[ORB_TOTAL_ELEM] = {};

// Determine the data range
static inline bool is_in_range(unsigned left, unsigned value, unsigned right) {
    if (right > left) {
        return (left <= value) && (value <= right);

    } else { // Maybe the data overflowed and a wraparound occurred
        return (left <= value) || (value <= right);
    }
}

// round up to nearest power of two
// Such as 0 => 1, 1 => 1, 2 => 2 ,3 => 4, 10 => 16, 60 => 64, 65...255 => 128
// Note: When the input value > 128, the output is always 128
static inline uint8_t round_pow_of_two_8(uint8_t n) {
    if (n == 0) {
        return 1;
    }

    // Avoid is already a power of 2
    uint8_t value = n - 1;

    // Fill 1
    value |= value >> 1U;
    value |= value >> 2U;
    value |= value >> 4U;

    // Unable to round-up, take the value of round-down
    if (value == UINT8_MAX) {
        value >>= 1U;
    }

    return value + 1;
}

bool uorb_device_node_exist(ORB_ID orb_id, uint8_t instance) {
    if (/*orb_id == ORB_ID::INVALID ||*/
        (instance > ORB_MULTI_MAX_INSTANCES)) {
        return false;
    }
    uint32_t idx = orb_id * ORB_MULTI_MAX_INSTANCES + instance;
    uint32_t grp = idx / ORB_BITS_PER_ELEM;
    uint32_t bit = idx % ORB_BITS_PER_ELEM;
    rt_enter_critical();
    bool exist = _uorb_device_exist[grp] & (1 << bit);
    rt_exit_critical();
    return exist;
}

void uorb_device_mark_exist(ORB_ID orb_id, uint8_t instance) {
    if (/*orb_id == ORB_ID::INVALID ||*/
        (instance > ORB_MULTI_MAX_INSTANCES)) {
        return;
    }

    uint32_t idx = orb_id * ORB_MULTI_MAX_INSTANCES + instance;
    uint32_t grp = idx / ORB_BITS_PER_ELEM;
    uint32_t bit = idx % ORB_BITS_PER_ELEM;
    rt_enter_critical();
    _uorb_device_exist[grp] |= (1 << bit);
    rt_exit_critical();
}

void uorb_device_clear_exist(ORB_ID orb_id, uint8_t instance) {
    if (/*orb_id == ORB_ID::INVALID ||*/
        (instance > ORB_MULTI_MAX_INSTANCES)) {
        return;
    }

    uint32_t idx = orb_id * ORB_MULTI_MAX_INSTANCES + instance;
    uint32_t grp = idx / ORB_BITS_PER_ELEM;
    uint32_t bit = idx % ORB_BITS_PER_ELEM;
    rt_enter_critical();
    _uorb_device_exist[grp] &= (~(1 << bit));
    rt_exit_critical();
}

void uorb_device_add_node(uorb_device_t node) {
    if (node) {
        rt_list_insert_before(&_uorb_device_list, &node->list);
    }
}

void uorb_device_remove_node(uorb_device_t node) {
    if (node) {
        rt_list_remove(&node->list);
    }
}

uorb_device_t uorb_device_find_node(const struct orb_metadata *meta, uint8_t instance) {
    if (!meta) {
        return NULL;
    }

    // 先看看是否创建
    if (!uorb_device_node_exist((ORB_ID)(meta->o_id), instance)) {
        return NULL;
    }

    // 遍历list查找节点
    rt_list_t *pos, *temp_list;
    rt_enter_critical();
    rt_list_for_each_safe(pos, temp_list, &_uorb_device_list) {
        uorb_device_t node = (uorb_device_t)pos;
        if ((rt_strcmp(node->meta->o_name, meta->o_name) == 0) && (node->instance == instance)) {
            rt_exit_critical();
            return node;
        }
    }
    rt_exit_critical();

    return NULL;
}

uint8_t uorb_device_get_queue_size(const uorb_device_t node) {
    if (!node) {
        return 0;
    }
    return node->queue_size;
}

uint8_t uorb_device_get_instance(const uorb_device_t node) {
    if (!node) {
        return -1;
    }

    return node->instance;
}

bool uorb_device_node_advertised(const uorb_device_t node) {
    if (node) {
        return node->advertised;
    }

    return false;
}

void uorb_device_mark_advertised(uorb_device_t node) {
    if (node) {
        node->advertised = true;
    }
}

int urob_device_set_queue_size(uorb_device_t node, uint32_t queue_size) {
    if (node->queue_size == queue_size) {
        return 0;
    }

    if (node->data || node->queue_size > queue_size || queue_size > 255) {
        return -1;
    }

    node->queue_size = round_pow_of_two_8(queue_size);
    return 0;
}

unsigned uorb_device_updates_available(const uorb_device_t node, unsigned last_generation) {
    if (node && node->advertised) {
        return (node->generation - last_generation);
    }
    return 0;
}

uorb_device_t uorb_device_add_internal_subscriber(ORB_ID orb_id, uint8_t instance, unsigned *initial_generation) {
    uorb_device_t node = uorb_device_find_node(get_orb_meta(orb_id), instance);
    if (node) {
        if (node->subscriber_count > 50) {
            LOG_W("%s's follows reach %d, maybe not clean older", node->meta->o_name, node->subscriber_count);
        }
        rt_enter_critical();
        node->subscriber_count++;
        *initial_generation = node->generation;
        rt_exit_critical();
    }
    return node;
}

void uorb_device_remove_internal_subscriber(uorb_device_t node) {
    if (node) {
        rt_enter_critical();

        node->subscriber_count--;
        rt_exit_critical();
    }
}

uorb_device_t uorb_device_create(const struct orb_metadata *meta, const uint8_t instance, uint8_t queue_size) {
    uorb_device_t node = rt_malloc(sizeof(struct uorb_device_node));

    if (!node) {
        return NULL;
    }

    node->meta             = meta;
    node->data             = NULL;
    node->data_valid       = false;
    node->generation       = 0;
    node->instance         = instance;
    node->advertised       = false;
    node->queue_size       = round_pow_of_two_8(queue_size);
    node->subscriber_count = 0;

    // 标记主题已经创建
    uorb_device_mark_exist(meta->o_id, instance);
    // 将主题节点添加到链表
    rt_list_insert_before(&_uorb_device_list, &node->list);

    return node;
}

int uorb_device_delete(uorb_device_t node) {
    // 主要订阅数大于0，就不允许删除节点
    if (!node || node->subscriber_count > 0) {
        return -1;
    }

    // 将节点从链表移除
    rt_list_remove(&node->list);
    // 标记主题不存在
    uorb_device_clear_exist(node->meta->o_id, node->instance);

    node->meta = NULL;
    // 释放data指针
    if (node->data) {
        void *data = node->data;
        node->data = NULL;
        rt_free(data);
    }
    node->data_valid       = false;
    node->generation       = 0;
    node->instance         = 0;
    node->advertised       = false;
    node->queue_size       = 0;
    node->subscriber_count = 0;
    // 释放node指针
    rt_free(node);

    return 0;
}

// 广告新主题，并指定queue_size
// 当instance=NULL是表示只广告instance=0的主题
// 当instance!=NULL则公告新主题，返回instance
uorb_device_t uorb_device_advertise(const struct orb_metadata *meta, const void *data, int *instance,
                                    unsigned int queue_size) {
    if (!meta) {
        return NULL;
    }

    orb_advert_t node = NULL;

    // 允许的最大instance个数
    int max_inst = ORB_MULTI_MAX_INSTANCES;
    int inst     = 0;

    // instance=NULL，表示只公告inst=0主题
    if (!instance) {
        node = uorb_device_find_node(meta, 0); //  查找inst=0主题
        if (node) {
            max_inst = 0;                      // 查找到不需要创建
        } else {
            max_inst = 1;                      // 没有查到到，后面代码创建
        }
    }

    // 搜索实例是否存在，不存在则创建，存在则判断是否公告
    for (inst = 0; inst < max_inst; inst++) {
        node = uorb_device_find_node(meta, inst);
        if (node) {
            if (!node->advertised) {
                break;
            }
        } else {
            node = uorb_device_create(meta, inst, queue_size);
            break;
        }
    }

    // 如果node找到/创建成功，发布首次数据，且返回instance
    if (node) {
        // 标记为已经公告，只有公告过的主题才能copy和publish数据
        node->advertised = true;
        if (data) {
            // orb_publish(meta, node, data);
            uorb_device_write(node, data);
        }
        // 返回inst
        if (instance) {
            *instance = inst;
        }
    }

    return node;
}

int uorb_device_unadvertise(orb_advert_t node) {
    if (!node) {
        return -1;
    }

    if (node->subscriber_count > 0) {
        node->advertised = false;
        return 0;
    }

    return uorb_device_delete(node);
}

int uorb_device_read(uorb_device_t node, void *dst, uint32_t *generation) {
    if ((dst != NULL) && (node->data != NULL)) {
        if (node->queue_size == 1) {
            rt_enter_critical();
            rt_memcpy(dst, node->data, node->meta->o_size);
            *generation = rt_atomic_load(&node->generation);
            rt_exit_critical();
            return node->meta->o_size;

        } else {
            rt_enter_critical();
            const unsigned current_generation = rt_atomic_load(&node->generation);

            if (current_generation == *generation) {
                /* The subscriber already read the latest message, but nothing new was published yet.
                 * Return the previous message
                 */
                --(*generation);
            }

            // Compatible with normal and overflow conditions
            if (!is_in_range(current_generation - node->queue_size, *generation, current_generation - 1)) {
                // Reader is too far behind: some messages are lost
                *generation = current_generation - node->queue_size;
            }

            rt_memcpy(dst, node->data + (node->meta->o_size * (*generation % node->queue_size)), node->meta->o_size);
            rt_exit_critical();

            ++(*generation);

            return node->meta->o_size;
        }
    }

    return 0;
}

int uorb_device_write(uorb_device_t node, const void *data) {
    if (!node || !data || !node->meta) {
        return 0;
    }

    if (!node->data) {
        // rt_enter_critical();
        node->data = rt_calloc(1, node->meta->o_size * node->queue_size);
        // rt_exit_critical();

        /* failed or could not allocate */
        if (!node->data) {
            return 0;
        }
    }

    /* Perform an atomic copy. */
    rt_enter_critical();
    /* wrap-around happens after ~49 days, assuming a publisher rate of 1 kHz */
    unsigned generation = rt_atomic_add(&node->generation, 1L); //.fetch_add(1);

    rt_memcpy(node->data + (node->meta->o_size * (generation % node->queue_size)), data, node->meta->o_size);

    // 注册callback
    // for (auto item : _callbacks) {
    //     item->call();
    // }

    /* Mark at least one data has been published */
    node->data_valid = true;

    rt_exit_critical();

    /* notify any poll waiters */
    // poll_notify(POLLIN);

    return node->meta->o_size;
}
