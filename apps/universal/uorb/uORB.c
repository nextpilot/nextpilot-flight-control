
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
#include "uorb_publication.h"
#include "uorb_subscription.h"

// 查找对应的主题是否存在，且已经公告
// 没有公告的主题是不能publish和copy数据
int orb_exists(const struct orb_metadata *meta, int instance) {
    if (!meta) {
        return -1;
    }

    if (instance < 0 || instance > (ORB_MULTI_MAX_INSTANCES - 1)) {
        return -1;
    }

    uorb_device_t node = uorb_device_get_node(meta, instance);

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

bool orb_is_advertised(const uorb_device_t node) {
    if (node && node->advertised) {
        return true;
    }

    return false;
}

uorb_device_t orb_add_internal_subscriber(ORB_ID orb_id, uint8_t instance, unsigned *initial_generation) {
    uorb_device_t node = uorb_device_get_node(get_orb_meta(orb_id), instance);
    if (node) {
        rt_enter_critical();
        node->subscriber_count++;
        *initial_generation = node->generation;
        rt_exit_critical();
    }
    return node;
}

void orb_remove_internal_subscriber(uorb_device_t node) {
    if (node) {
        rt_enter_critical();
        node->subscriber_count--;
        rt_exit_critical();
    }
}

unsigned orb_updates_available(uorb_device_t node, unsigned last_generation) {
    if (node && node->advertised) {
        return (node->generation - last_generation);
    }
    return 0;
}

uint8_t orb_get_queue_size(const uorb_device_t node) {
    if (!node) {
        return 0;
    }
    return node->queue_size;
}

uint8_t orb_get_instance(const uorb_device_t node) {
    if (!node) {
        return -1;
    }

    return node->instance;
}

bool orb_device_node_exists(ORB_ID orb_id, uint8_t instance) {
    return uorb_device_is_exist(orb_id, instance);
}

static bool orb_data_copy(uorb_device_t node, void *dst, uint32_t *generation, bool only_if_updated) {
    if (!orb_is_advertised(node)) {
        return false;
    }

    if (only_if_updated &&
        !orb_updates_available(node, *generation)) {
        return false;
    }

    // return uorb_device_copy(node, dst, generation);
    return true;
}

orb_advert_t orb_advertise_multi(const struct orb_metadata *meta, const void *data, int *instance,
                                 unsigned int queue_size) {
    return 0;
}

int orb_unadvertise(orb_advert_t node) {
    return uorb_device_unadvertise(node);
}

int orb_publish(const struct orb_metadata *meta, orb_advert_t node, const void *data) {
    return uorb_device_publish(node, data);
}

static bool orb_search_node(orb_subptr_t sub) {
    if (!sub) {
        return false;
    }
    // 如果node不为空，则不需要查找了
    if (sub->_node) {
        return true;
    }

    unsigned      initial_generation = 0;
    uorb_device_t node               = orb_add_internal_subscriber(sub->_orb_id, sub->_instance, &initial_generation);

    if (node) {
        sub->_node            = node;
        sub->_last_generation = initial_generation;
        return true;
    }
    return false;
}

orb_subval_t orb_subscribe_multi(const struct orb_metadata *meta, uint8_t instance) {
    orb_subval_t sub;
    if (meta) {
        sub._orb_id          = meta->o_id;
        sub._node            = NULL;
        sub._instance        = instance;
        sub._last_generation = 0;

        orb_search_node(&sub);
    }

    return sub;
}

orb_subval_t orb_subscribe(const struct orb_metadata *meta) {
    return orb_subscribe_multi(meta, 0);
}

int orb_unsubscribe(orb_subptr_t sub) {
    if (!sub) {
        return -1;
    }

    if (sub->_node) {
        orb_remove_internal_subscriber(sub->_node);
    }

    sub->_node            = NULL;
    sub->_last_generation = 0;

    return 0;
}

// 检查数据是否更新
int orb_check(orb_subptr_t sub, bool *updated) {
    if (!sub || !updated) {
        return -1;
    }

    // 获取node
    if (!sub->_node) {
        orb_search_node(sub);
    }

    if (!sub->_node) {
        return -1;
    }

    unsigned available = orb_updates_available(sub->_node, sub->_last_generation);

    *updated = available != 0;

    return 0;
}

// 强制拷贝，不管是否更新都拷贝
int orb_copy(const struct orb_metadata *meta, orb_subptr_t sub, void *buffer) {
    if (!sub || !meta || !buffer) {
        return -1;
    }

    if (sub->_orb_id != meta->o_id) {
        return -1;
    }

    if (sub->_node) {
        return orb_data_copy(sub->_node, buffer, &sub->_last_generation, false) ? 0 : -1;
    }

    return -1;
}

// 更新了才拷贝
int orb_update(orb_subptr_t sub, void *data) {
    if (!sub || !data) {
        return -1;
    }

    if (!sub->_node) {
        orb_search_node(sub);
    }

    if (sub->_node) {
        return orb_data_copy(sub->_node, data, &sub->_last_generation, true) ? 0 : -1;
    }

    return -1;
}

int orb_poll(orb_subptr_t sub, int timeout_us) {
    return 0;
}

bool orb_change_instance(orb_subptr_t sub, uint8_t instance) {
    if (!sub || sub->_instance == instance) {
        return false;
    }

    // 取消订阅
    orb_unsubscribe(sub);

    // 查找新的node
    sub->_instance                   = instance;
    unsigned      initial_generation = 0;
    uorb_device_t node               = orb_add_internal_subscriber(sub->_orb_id, sub->_instance, &initial_generation);
    sub->_node                       = node;
    sub->_last_generation            = initial_generation;

    return true;
}

int orb_set_interval(orb_subptr_t sub, unsigned interval) {
    return 0;
}

int orb_get_interval(orb_subptr_t sub, unsigned *interval) {
    return 0;
}

bool orb_register_callback(uorb_device_t node) {
    return 0;
}

bool orb_unregister_callback(uorb_device_t node) {
    return 0;
}
