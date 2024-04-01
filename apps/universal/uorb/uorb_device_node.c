
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

#define ATOMIC_ENTER rt_enter_critical()
#define ATOMIC_LEAVE rt_exit_critical()

// #define ORB_MULTI_MAX_INSTANCES 4
#define ORB_TOPICS_COUNT 10

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

bool uorb_device_is_exist(ORB_ID orb_id, uint8_t instance) {
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

void uorb_device_set_exist(ORB_ID orb_id, uint8_t instance) {
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

uorb_device_t uorb_device_create(const struct orb_metadata *meta, const uint8_t instance, uint8_t queue_size) {
    uorb_device_t node = rt_malloc(sizeof(struct uorb_device_node));

    if (!node) {
        return NULL;
    }

    node->_meta             = meta;
    node->_data             = NULL;
    node->_data_valid       = false;
    node->_generation       = 0;
    node->_instance         = instance;
    node->_advertised       = false;
    node->_queue_size       = round_pow_of_two_8(queue_size);
    node->_subscriber_count = 0;

    // 标记主题已经创建
    uorb_device_set_exist(meta->o_id, instance);
    // 将主题节点添加到链表
    rt_list_insert_before(&_uorb_device_list, &node->_list);

    return node;
}

int uorb_device_delete(uorb_device_t node) {
    // 主要订阅数大于0，就不允许删除节点
    if (!node || node->_subscriber_count > 0) {
        return -1;
    }

    // 将节点从链表移除
    rt_list_remove(&node->_list);
    // 标记主题不存在
    uorb_device_clear_exist(node->_meta->o_id, node->_instance);

    node->_meta = NULL;
    // 释放data指针
    if (node->_data) {
        void *data  = node->_data;
        node->_data = NULL;
        rt_free(data);
    }
    node->_data_valid       = false;
    node->_generation       = 0;
    node->_instance         = 0;
    node->_advertised       = false;
    node->_queue_size       = 0;
    node->_subscriber_count = 0;
    // 释放node指针
    rt_free(node);

    return 0;
}

bool uorb_device_copy(uorb_device_t node, void *dst, uint32_t *generation) {
    if ((dst != NULL) && (node->_data != NULL)) {
        if (node->_queue_size == 1) {
            ATOMIC_ENTER;
            rt_memcpy(dst, node->_data, node->_meta->o_size);
            *generation = rt_atomic_load(&node->_generation);
            ATOMIC_LEAVE;
            return true;

        } else {
            ATOMIC_ENTER;
            const unsigned current_generation = rt_atomic_load(&node->_generation);

            if (current_generation == *generation) {
                /* The subscriber already read the latest message, but nothing new was published yet.
                 * Return the previous message
                 */
                --(*generation);
            }

            // Compatible with normal and overflow conditions
            if (!is_in_range(current_generation - node->_queue_size, *generation, current_generation - 1)) {
                // Reader is too far behind: some messages are lost
                *generation = current_generation - node->_queue_size;
            }

            rt_memcpy(dst, node->_data + (node->_meta->o_size * (*generation % node->_queue_size)), node->_meta->o_size);
            ATOMIC_LEAVE;

            ++(*generation);

            return true;
        }
    }

    return false;
}

bool uorb_device_publish(uorb_device_t node, const void *data) {
    if (!node || !data || !node->_meta) {
        return false;
    }

    if (!node->_data) {
        node->_data = rt_malloc(node->_meta->o_size * node->_queue_size);

        if (!node->_data) {
            return false;
        }
    }

    node->_data_valid = true;
    return true;
}

int urob_device_update_queue_size(uorb_device_t node, uint32_t queue_size) {
    if (node->_queue_size == queue_size) {
        return 0;
    }

    if (node->_data || node->_queue_size > queue_size || queue_size > 255) {
        return -1;
    }

    node->_queue_size = round_pow_of_two_8(queue_size);
    return 0;
}

uorb_device_t uorb_device_get_node(const struct orb_metadata *meta, uint8_t instance) {
    if (!meta) {
        return NULL;
    }

    // 先看看是否创建
    if (!uorb_device_is_exist((ORB_ID)(meta->o_id), instance)) {
        return NULL;
    }

    // 遍历list查找节点
    rt_list_t *pos, *temp_list;
    rt_enter_critical();
    rt_list_for_each_safe(pos, temp_list, &_uorb_device_list) {
        uorb_device_t node = (uorb_device_t)pos;
        if ((rt_strcmp(node->_meta->o_name, meta->o_name) == 0) &&
            (node->_instance == instance)) {
            rt_exit_critical();
            return node;
        }
    }
    rt_exit_critical();

    return NULL;
}
