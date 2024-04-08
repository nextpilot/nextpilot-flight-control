#include "uORBPublication.hpp"
#include "uORBDeviceNode.hpp"
#include "uORB.h"

using namespace nextpilot::uORB;

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

// 广告新主题，并指定queue_size
// 当instance=NULL是表示只广告instance=0的主题
// 当instance!=NULL则公告新主题，返回instance
orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance, unsigned int queue_size) {
    return DeviceNode::advertise(meta, data, instance, queue_size);
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
    return DeviceNode::unadvertise((DeviceNode *)node);
}

int orb_publish(const struct orb_metadata *meta, orb_advert_t handle, const void *data) {
    if (!data) {
        return -1;
    }

    DeviceNode *node = (DeviceNode *)handle;

    // 如果没有指定节点，则使用instance=0的节点
    if (meta && node) {
        if (meta != node->get_meta()) {
            return -1;
        } else {
            return node->write(data);
        }
    } else if (!meta && node) {
        return node->write(data);
    } else if (meta && !node) {
        node = DeviceNode::getDeviceNode(meta, 0);
        return node->write(data);
    } else {
        return -1;
    }
}

// int orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *node, const void *data, int *instance) {
//     if (!*node) {
//         *node = orb_advertise_multi(meta, data, instance);
//         if (*node) {
//             return 0;
//         }
//     } else {
//         return orb_publish(meta, *node, data);
//     }

//     return -1;
// }
