/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "uORBSubscription.hpp"
#include "uORBDeviceNode.hpp"
#include "uORB.h"

using namespace nextpilot::uORB;

// 查找对应的主题是否存在，且已经公告，没有公告advertised=false主题不能publish和copy数据
int orb_exists(const struct orb_metadata *meta, int instance) {
    if (!meta) {
        return -1;
    }

    // instance valid range: [0, ORB_MULTI_MAX_INSTANCES)
    if (instance < 0 || instance > (ORB_MULTI_MAX_INSTANCES - 1)) {
        return -1;
    }

    DeviceNode *node = DeviceNode::getDeviceNode(meta, instance);

    if (node && node->is_advertised()) {
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

orb_subscr_t orb_subscribe_multi(const struct orb_metadata *meta, uint8_t instance) {
    return new SubscriptionInterval(meta, 0, instance);
}

orb_subscr_t orb_subscribe(const struct orb_metadata *meta) {
    return new SubscriptionInterval(meta, 0, 0);
}

int orb_unsubscribe(orb_subscr_t handle) {
    if (!handle) {
        return -1;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    sub->unsubscribe();

    delete sub;

    return 0;
}

// 检查数据是否更新
int orb_check(orb_subscr_t handle, bool *updated) {
    if (!handle || !updated) {
        return -1;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    *updated = sub->updated();
    return 0;
}

// 强制拷贝，不管是否更新都拷贝
int orb_copy(const struct orb_metadata *meta, orb_subscr_t handle, void *buffer) {
    if (!buffer) {
        return -1;
    }

    if (!meta && !handle) {
        return -1;
    } else if (meta && !handle) { // 拷贝instance=0
        DeviceNode *node = DeviceNode::getDeviceNode(meta, 0);
        if (node && node->is_advertised()) {
            unsigned generation;
            node->read(buffer, generation);
            return 0;
        }
        return -1;
    } else if (!meta && handle) { // 直接拷贝
        SubscriptionInterval *sub = (SubscriptionInterval *)handle;
        return sub->copy(buffer) ? 0 : -1;
    } else { // 校验meta之后再拷贝
        SubscriptionInterval *sub = (SubscriptionInterval *)handle;
        if (sub->get_topic() == meta) {
            return sub->copy(buffer) ? 0 : -1;
        }
        return -1;
    }
}

// 更新了才拷贝
int orb_update(orb_subscr_t handle, void *data) {
    if (!handle || !data) {
        return -1;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    return sub->update(data) ? 0 : -1;
}

// int orb_poll(orb_subscr_t sub, int timeout_us) {
//     return 0;
// }

// int orb_change_instance(orb_subscr_t handle, uint8_t instance) {
//     // if (!sub || sub->get_instance() == instance) {
//     //     return -1;
//     // }

//     // // 取消订阅
//     // orb_unsubscribe(sub);

//     // // 查找新的node
//     // sub->instance                  = instance;
//     // unsigned    initial_generation = 0;
//     // DeviceNode *node               = uorb_device_add_internal_subscriber(sub->orb_id, sub->instance, &initial_generation);
//     // sub->node                      = node;
//     // sub->last_generation           = initial_generation;

//     return 0;
// }

int orb_set_interval(orb_subscr_t handle, unsigned interval_ms) {
    if (!handle) {
        return -1;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    sub->set_interval_us(interval_ms * 1000UL);
    return 0;
}

int orb_get_interval(orb_subscr_t handle, unsigned *interval_ms) {
    if (!handle || !interval_ms) {
        return -1;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;
    *interval_ms              = sub->get_interval_us() / 1000UL;

    return 0;
}
