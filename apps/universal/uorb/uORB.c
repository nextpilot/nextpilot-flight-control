
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

    // instance valid range: [0, ORB_MULTI_MAX_INSTANCES)
    if (instance < 0 || instance > (ORB_MULTI_MAX_INSTANCES - 1)) {
        return -1;
    }

    uorb_device_t node = uorb_device_get_node(meta, instance);

    if (node && node->advertised) {
        return 0;
    }

    return -1;
}

// 注意跟orb_exists的区别
// orb_device_node_exists仅仅判断节点是否存在
// orb_exist需要还需判断节点是否advertised的
bool orb_device_node_exists(ORB_ID orb_id, uint8_t instance) {
    return uorb_device_is_exist(orb_id, instance);
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

// 广告新主题，并指定queue_size
// 当instance=NULL是表示只广告instance=0的主题
// 当instance!=NULL则公告新主题，返回instance
orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance,
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
        node = uorb_device_get_node(meta, 0); //  查找inst=0主题
        if (node) {
            max_inst = 0; // 查找到不需要创建
        } else {
            max_inst = 1; // 没有查到到，后面代码创建
        }
    }

    // 搜索实例是否存在，不存在则创建，存在则判断是否公告
    for (inst = 0; inst < max_inst; inst++) {
        node = uorb_device_get_node(meta, inst);
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
            orb_publish(meta, node, data);
        }
        // 返回inst
        if (instance) {
            *instance = inst;
        }
    }

    return node;
}

// 广告新主题，使用默认queue_size=1
// 当instance=NULL是表示只广告instance=0的主题
// 当instance!=NULL则公告新主题，返回instance
orb_advert_t orb_advertise_multi(const struct orb_metadata *meta, const void *data, int *instance) {
    return orb_advertise_multi_queue(meta, data, instance, 1);
}

// 广告instance=0的主题，并指定queue_size
orb_advert_t orb_advertise_queue(const struct orb_metadata *meta, const void *data,
                                 unsigned int queue_size) {
    return orb_advertise_multi_queue(meta, data, NULL, queue_size);
}

// 广告instance = 0的主题使用默认queue_size=1
orb_advert_t orb_advertise(const struct orb_metadata *meta, const void *data) {
    return orb_advertise_multi_queue(meta, data, NULL, 1);
}

int orb_unadvertise(orb_advert_t node) {
    if (!node) {
        return -1;
    }

    node->advertised = false;
    return 0;
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
