/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

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

bool orb_device_node_exists(ORB_ID orb_id, uint8_t instance) {
    return DeviceNode::deviceNodeExists(orb_id, instance);
}

int orb_group_count(const struct orb_metadata *meta) {
    uint8_t instance = 0;

    while (orb_exists(meta, instance) == 0) {
        instance++;
    }
    return instance;
}

uint8_t orb_get_queue_size(const orb_advert_t node) {
    if (node) {
        return static_cast<const DeviceNode *>(node)->get_queue_size();
    }
    return 0;
}

uint8_t orb_get_instance(const orb_advert_t node) {
    if (node) {
        return static_cast<const DeviceNode *>(node)->get_instance();
    }
    return 0;
}

void *orb_add_internal_subscriber(ORB_ID orb_id, uint8_t instance, unsigned *initial_generation) {
    DeviceNode *node = DeviceNode::getDeviceNode(get_orb_meta(orb_id), instance);
    if (node) {
        node->add_internal_subscriber();
        *initial_generation = node->get_initial_generation();
    }
    return node;
}

void orb_remove_internal_subscriber(void *node) {
    if (node) {
        static_cast<DeviceNode *>(node)->remove_internal_subscriber();
    }
}

bool orb_is_advertised(const void *node) {
    if (!node) {
        return static_cast<const DeviceNode *>(node)->is_advertised();
    }
    return false;
}

unsigned orb_updates_available(const void *node, unsigned last_generation) {
    return orb_is_advertised(node) ? static_cast<const DeviceNode *>(node)->updates_available(last_generation) : 0;
}

bool orb_data_copy(void *node_handle, void *dst, unsigned &generation, bool only_if_updated) {
    return false;
}
