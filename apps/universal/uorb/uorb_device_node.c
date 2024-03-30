
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

#define ORB_MULTI_MAX_INSTANCES 4
#define ORB_TOPICS_COUNT        10

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
    uint32_t idx = orb_id * ORB_MULTI_MAX_INSTANCES + instance;
    uint32_t grp = idx / ORB_BITS_PER_ELEM;
    uint32_t bit = idx % ORB_BITS_PER_ELEM;

    return _uorb_device_exist[grp] & (1 << bit);
}

void uorb_device_set_exist(ORB_ID orb_id, uint8_t instance) {
    uint32_t idx = orb_id * ORB_MULTI_MAX_INSTANCES + instance;
    uint32_t grp = idx / ORB_BITS_PER_ELEM;
    uint32_t bit = idx % ORB_BITS_PER_ELEM;

    _uorb_device_exist[grp] |= (1 << bit);
}

int uorb_device_init(uorb_device_t node) {
    if (!node) {
        return -1;
    }

    node->meta             = NULL;
    node->data             = NULL;
    node->data_valid       = false;
    node->generation       = 0;
    node->instance         = 0;
    node->advertised       = false;
    node->queue_size       = 1;
    node->subscriber_count = 0;

    return 0;
}

bool uorb_device_copy(uorb_device_t node, void *dst, uint32_t *generation) {
    if ((dst != NULL) && (node->data != NULL)) {
        if (node->queue_size == 1) {
            ATOMIC_ENTER;
            memcpy(dst, node->data, node->meta->o_size);
            *generation = rt_atomic_load(&node->generation);
            ATOMIC_LEAVE;
            return true;

        } else {
            ATOMIC_ENTER;
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

            memcpy(dst, node->data + (node->meta->o_size * (*generation % node->queue_size)), node->meta->o_size);
            ATOMIC_LEAVE;

            ++(*generation);

            return true;
        }
    }

    return false;
}

bool uorb_device_is_advertised(uorb_device_t node) {
    return node->advertised;
}

void uorb_device_mark_as_advertised(uorb_device_t node) {
    node->advertised = true;
}

bool uorb_device_publish(uorb_device_t node, const void *data) {
    if (!node || !data || !node->meta) {
        return false;
    }

    if (!node->data) {
        node->data = rt_malloc(node->meta->o_size * node->queue_size);

        if (!node->data) {
            return false;
        }
    }

    node->data_valid = true;
    return true;
}

int uorb_device_unadvertise(uorb_device_t node) {
    if (!node) {
        return -1;
    }

    node->advertised = false;
    return;
}

void uorb_device_add_internal_subscriber(uorb_device_t node) {
    rt_enter_critical();
    node->subscriber_count++;
    rt_exit_critical();
}

void uorb_device_remove_internal_subscribe(uorb_device_t node) {
    rt_enter_critical();
    node->subscriber_count--;
    rt_exit_critical();
}

int urob_device_update_queue_size(uorb_device_t node, uint32_t queue_size) {
    if (node->queue_size == queue_size) {
        return 0;
    }

    if (node->data || node->queue_size > queue_size || queue_size > 255) {
        return -1;
    }

    node->queue_size = round_pow_of_two_8(queue_size);
    return 0;
}

uint32_t uorb_device_get_initial_generation(uorb_device_t node) {
    uint32_t generation = rt_atomic_load(&node->generation) - (node->data_valid ? 1 : 0);
    return generation;
}

bool uorb_device_register_callback(uorb_device_t node, void *callback_sub) {
    if (!callback_sub) {
        return false;
    }
}

uorb_device_t *uorb_device_get_node(const struct orb_metadata *meta, uint8_t instance) {
    if (!meta) {
        return NULL;
    }

    if (!uorb_device_is_exist((ORB_ID)(meta->o_id), instance)) {
        return NULL;
    }

    rt_list_t *pos, *temp_list;

    rt_enter_critical();
    rt_list_for_each_safe(pos, temp_list, &_uorb_device_list) {
        uorb_device_t node = (uorb_device_t)pos;
        if ((rt_strcmp(node->meta->o_name, meta->o_name) == 0) &&
            (node->instance == instance)) {
            rt_exit_critical();
            return node;
        }
    }
    rt_exit_critical();

    return NULL;
}

// int uorb_device_advertise_single(const struct orb_metadata *meta, int instance uint8_t queue_size) {
// }
