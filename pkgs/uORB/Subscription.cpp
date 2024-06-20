/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "Subscription.hpp"
#include "DeviceNode.hpp"
#include "uORB.h"

using namespace nextpilot::uORB;

// 查找对应的主题是否存在，且已经公告，没有公告advertised=false主题不能publish和copy数据
int orb_exists(const struct orb_metadata *meta, int instance) {
    if (!meta) {
        return -RT_ERROR;
    }

    // instance valid range: [0, ORB_MULTI_MAX_INSTANCES)
    if (instance < 0 || instance > (ORB_MULTI_MAX_INSTANCES - 1)) {
        return -RT_ERROR;
    }

    DeviceNode *node = DeviceNode::getDeviceNode(meta, instance);

    if (node && node->is_advertised()) {
        return RT_EOK;
    }

    return -RT_ERROR;
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
        return -RT_ERROR;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    sub->unsubscribe();

    delete sub;

    return RT_EOK;
}

// 检查数据是否更新
int orb_check(orb_subscr_t handle, bool *updated) {
    if (!handle || !updated) {
        return -RT_ERROR;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    *updated = sub->updated();
    return RT_EOK;
}

// 强制拷贝，不管是否更新都拷贝
int orb_copy(const struct orb_metadata *meta, orb_subscr_t handle, void *buffer) {
    if (!buffer) {
        return -RT_ERROR;
    }

    if (!meta && !handle) {
        return -RT_ERROR;
    } else if (meta && !handle) { // 拷贝instance=0
        DeviceNode *node = DeviceNode::getDeviceNode(meta, 0);
        if (node && node->is_advertised()) {
            unsigned generation;
            node->read(buffer, generation);
            return RT_EOK;
        }
        return -RT_ERROR;
    } else if (!meta && handle) { // 直接拷贝
        SubscriptionInterval *sub = (SubscriptionInterval *)handle;
        return sub->copy(buffer) ? RT_EOK : -RT_ERROR;
    } else { // 校验meta之后再拷贝
        SubscriptionInterval *sub = (SubscriptionInterval *)handle;
        if (sub->get_topic() == meta) {
            return sub->copy(buffer) ? RT_EOK : -RT_ERROR;
        }
        return -RT_ERROR;
    }
}

// 更新了才拷贝
int orb_update(orb_subscr_t handle, void *data) {
    if (!handle || !data) {
        return -RT_ERROR;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    return sub->update(data) ? RT_EOK : -RT_ERROR;
}

int orb_poll(orb_subscr_t *handle[], int len, int timeout_us) {
    int ret = -RT_ERROR;

    SubscriptionPolling *sub = (SubscriptionPolling *)handle;

    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  _cv    = PTHREAD_COND_INITIALIZER;

    pthread_mutexattr_t attr;
    int                 ret_attr_init  = pthread_mutexattr_init(&attr);
    int                 ret_mutex_init = pthread_mutex_init(&_mutex, &attr);

    for (int i = 0; i < len; i++) {
        sub[i].register_callback(&_cv);
    }

    pthread_mutex_lock(&_mutex);
    if (timeout_us == 0) {
        // wait with no timeout
        ret = pthread_cond_wait(&_cv, &_mutex);
    } else {
        // otherwise wait with timeout based on interval
        struct timespec ts;
        ret = pthread_cond_timedwait(&_cv, &_mutex, &ts);
    }

    for (int i = 0; i < len; i++) {
        sub[i].unregister_callback();
    }
    pthread_mutex_unlock(&_mutex);

    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cv);

    return ret;
}

// int orb_change_instance(orb_subscr_t handle, uint8_t instance) {
//     // if (!sub || sub->get_instance() == instance) {
//     //     return -RT_ERROR;
//     // }

//     // // 取消订阅
//     // orb_unsubscribe(sub);

//     // // 查找新的node
//     // sub->instance                  = instance;
//     // unsigned    initial_generation = 0;
//     // DeviceNode *node               = uorb_device_add_internal_subscriber(sub->orb_id, sub->instance, &initial_generation);
//     // sub->node                      = node;
//     // sub->last_generation           = initial_generation;

//     return RT_EOK;
// }

int orb_set_interval(orb_subscr_t handle, unsigned interval_ms) {
    if (!handle) {
        return -RT_ERROR;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;

    sub->set_interval_us(interval_ms * 1000UL);
    return RT_EOK;
}

int orb_get_interval(orb_subscr_t handle, unsigned *interval_ms) {
    if (!handle || !interval_ms) {
        return -RT_ERROR;
    }

    SubscriptionInterval *sub = (SubscriptionInterval *)handle;
    *interval_ms              = sub->get_interval_us() / 1000UL;

    return RT_EOK;
}
