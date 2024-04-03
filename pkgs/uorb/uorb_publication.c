#include "uorb_publication.h"
#include "uorb_device_node.h"
#include "uORB.h"

uint8_t orb_get_queue_size(const uorb_device_t node) {
    if (node) {
        return node->_queue_size;
    }
    return 0;
}

uint8_t orb_get_instance(const uorb_device_t node) {
    if (node) {
        return node->_instance;
    }
    return 0;
}

// 广告新主题，并指定queue_size
// 当instance=NULL是表示只广告instance=0的主题
// 当instance!=NULL则公告新主题，返回instance
orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance,
                                       unsigned int queue_size) {
    return uorb_device_advertise(meta, data, instance, queue_size);
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
    return uorb_device_unadvertise(node);
}

int orb_publish(const struct orb_metadata *meta, orb_advert_t node, const void *data) {
    // 如果没有指定节点，则使用instance=0的节点
    if (meta && node) {
        if (meta != node->_meta) {
            return -1;
        } else {
            return uorb_device_write(node, data);
        }
    } else if (!meta && node) {
        return uorb_device_write(node, data);
    } else if (meta && !node) {
        orb_advert_t node = uorb_device_find_node(meta, 0);
        return uorb_device_write(node, data);
    } else {
        return -1;
    }
}

int orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *node, const void *data, int *instance) {
    if (!*node) {
        *node = orb_advertise_multi(meta, data, instance);
        if (*node) {
            return 0;
        }
    } else {
        return orb_publish(meta, *node, data);
    }

    return -1;
}
