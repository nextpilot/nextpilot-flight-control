/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "uorb_subscription.h"
#include "uorb_device_node.h"
#include "uORB.h"
#include <stdbool.h>

// 查找对应的主题是否存在，且已经公告，没有公告advertised=false主题不能publish和copy数据
rt_err_t orb_exists(const struct orb_metadata *meta, int instance) {
    if (!meta) {
        return -1;
    }

    // instance valid range: [0, ORB_MULTI_MAX_INSTANCES)
    if (instance < 0 || instance > (ORB_MULTI_MAX_INSTANCES - 1)) {
        return -1;
    }

    uorb_device_t node = uorb_device_find_node(meta, instance);

    if (node && node->advertised) {
        return 0;
    }

    return -1;
}

int orb_group_count(const struct orb_metadata *meta) {
    uint8_t instance = 0;

    while (orb_exists(meta, instance) == 0) {
        instance++;
    }
    return instance;
}

bool orb_data_copy(uorb_device_t node, void *dst, uint32_t *generation, bool only_if_updated) {
    if (!uorb_device_node_advertised(node)) {
        return false;
    }

    if (only_if_updated &&
        !uorb_device_updates_available(node, *generation)) {
        return false;
    }

    return uorb_device_read(node, dst, generation);
}

static bool orb_subscribe_find_node(orb_subptr_t sub) {
    if (!sub) {
        return false;
    }
    // 如果node不为空，则不需要查找了
    if (sub->node) {
        return true;
    }

    unsigned      initial_generation = 0;
    uorb_device_t node               = uorb_device_add_internal_subscriber(sub->orb_id, sub->instance, &initial_generation);

    if (node) {
        sub->node            = node;
        sub->last_generation = initial_generation;
        return true;
    }
    return false;
}

orb_subval_t orb_subscribe_multi(const struct orb_metadata *meta, uint8_t instance) {
    orb_subval_t sub;
    if (meta) {
        sub.orb_id          = meta->o_id;
        sub.node            = NULL;
        sub.instance        = instance;
        sub.last_generation = 0;

        orb_subscribe_find_node(&sub);
    }

    return sub;
}

orb_subval_t orb_subscribe(const struct orb_metadata *meta) {
    return orb_subscribe_multi(meta, 0);
}

rt_err_t orb_unsubscribe(orb_subptr_t sub) {
    if (!sub) {
        return -1;
    }

    if (sub->node) {
        uorb_device_remove_internal_subscriber(sub->node);
    }

    sub->node            = NULL;
    sub->last_generation = 0;

    return 0;
}

// 检查数据是否更新
rt_err_t orb_check(orb_subptr_t sub, bool *updated) {
    if (!sub || !updated) {
        return -1;
    }

    // 获取node
    if (!sub->node) {
        orb_subscribe_find_node(sub);
    }

    if (!sub->node) {
        return -1;
    }

    unsigned available = uorb_device_updates_available(sub->node, sub->last_generation);

    *updated = available != 0;

    return 0;
}

// 强制拷贝，不管是否更新都拷贝
rt_err_t orb_copy(const struct orb_metadata *meta, orb_subptr_t sub, void *buffer) {
    if (!sub || !meta || !buffer) {
        return -1;
    }

    if (sub->orb_id != meta->o_id) {
        return -1;
    }

    if (sub->node) {
        return orb_data_copy(sub->node, buffer, &sub->last_generation, false) ? 0 : -1;
    }

    return -1;
}

// 更新了才拷贝
rt_err_t orb_update(orb_subptr_t sub, void *data) {
    if (!sub || !data) {
        return -1;
    }

    if (!sub->node) {
        orb_subscribe_find_node(sub);
    }

    if (sub->node) {
        return orb_data_copy(sub->node, data, &sub->last_generation, true) ? 0 : -1;
    }

    return -1;
}

rt_err_t orb_poll(orb_subptr_t sub, int timeout_us) {
    return 0;
}

rt_err_t orb_change_instance(orb_subptr_t sub, uint8_t instance) {
    if (!sub || sub->instance == instance) {
        return -1;
    }

    // 取消订阅
    orb_unsubscribe(sub);

    // 查找新的node
    sub->instance                    = instance;
    unsigned      initial_generation = 0;
    uorb_device_t node               = uorb_device_add_internal_subscriber(sub->orb_id, sub->instance, &initial_generation);
    sub->node                        = node;
    sub->last_generation             = initial_generation;

    return 0;
}

int orb_set_interval(orb_subptr_t sub, unsigned interval) {
    return 0;
}

int orb_get_interval(orb_subptr_t sub, unsigned *interval) {
    return 0;
}

rt_err_t orb_register_callback(uorb_device_t node) {
    return 0;
}

rt_err_t orb_unregister_callback(uorb_device_t node) {
    return 0;
}
