#include "Publication.hpp"
#include "DeviceNode.hpp"
#include "uORB.h"

using namespace nextpilot::uORB;

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
    return orb_advertise_multi_queue(meta, data, nullptr, queue_size);
}

// 广告instance = 0的主题使用默认queue_size=1
orb_advert_t orb_advertise(const struct orb_metadata *meta, const void *data) {
    return orb_advertise_multi_queue(meta, data, nullptr, 1);
}

int orb_unadvertise(orb_advert_t node) {
    return DeviceNode::unadvertise((DeviceNode *)node);
}

int orb_publish(const struct orb_metadata *meta, orb_advert_t handle, const void *data) {
    // 如果handle不存在，先查找meta对应的第0个节点
    // if (!handle) {
    //     handle = DeviceNode::getDeviceNode(meta, 0);
    // }
    // 如果没有找到，则创建meta的第0个节点
    // if (!handle) {
    //     handle = DeviceNode::advertise(meta, data, nullptr, 1);
    // }

    return DeviceNode::publish(meta, handle, data);
}

// int orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *node, const void *data, int *instance) {
//     if (!*node) {
//         *node = orb_advertise_multi(meta, data, instance);
//         if (*node) {
//             return RT_EOK;
//         }
//     } else {
//         return orb_publish(meta, *node, data);
//     }

//     return -RT_ERROR;
// }
