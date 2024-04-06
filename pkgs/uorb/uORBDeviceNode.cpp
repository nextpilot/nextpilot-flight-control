/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "uORBDeviceNode.hpp"
// #include "uorb_subscription.h"

namespace nextpilot::uORB {

DeviceNode *DeviceNode::advertise(const struct orb_metadata *meta, const void *data, int *instance,
                                  uint8_t queue_size) {
    if (!meta) {
        return nullptr;
    }

    DeviceNode *node = nullptr;

    // 允许的最大instance个数
    int max_inst = ORB_MULTI_MAX_INSTANCES;
    int inst     = 0;

    // instance=NULL，表示只公告inst=0主题
    if (!instance) {
        node = getDeviceNode(meta, 0); //  查找inst=0主题
        if (node) {
            max_inst = 0; // 查找到不需要创建
        } else {
            max_inst = 1; // 没有查到到，后面代码创建
        }
    }

    // 搜索实例是否存在，不存在则创建，存在则判断是否公告
    for (inst = 0; inst < max_inst; inst++) {
        node = getDeviceNode(meta, inst);
        if (node) {
            if (!node->_advertised) {
                break;
            }
        } else {
            node = new DeviceNode(meta, inst, queue_size);
            break;
        }
    }

    // 如果node找到/创建成功，发布首次数据，且返回instance
    if (node) {
        // 标记为已经公告，只有公告过的主题才能copy和publish数据
        node->_advertised = true;
        if (data) {
            // orb_publish(meta, node, data);
            node->write(data);
        }
        // 返回inst
        if (instance) {
            *instance = inst;
        }
    }

    return node;
}

bool DeviceNode::unadvertise(DeviceNode *node) {
    if (!node) {
        return -1;
    }

    node->_advertised = false;

    if (node->_subscriber_count > 0) {
        return 0;
    }

    delete node;

    return 0;
}

DeviceNode *DeviceNode::getDeviceNodeLocked(const struct orb_metadata *meta, const uint8_t instance) {
    rt_enter_critical();
    for (DeviceNode *node : _node_list) {
        if ((rt_strcmp(node->get_name(), meta->o_name) == 0) && (node->get_instance() == instance)) {
            rt_exit_critical();
            return node;
        }
    }
    rt_exit_critical();

    return nullptr;
}

DeviceNode *DeviceNode::getDeviceNode(const struct orb_metadata *meta, const uint8_t instance) {
    if (meta == nullptr) {
        return nullptr;
    }

    if (!deviceNodeExists(static_cast<ORB_ID>(meta->o_id), instance)) {
        return nullptr;
    }

    DeviceNode *node = getDeviceNodeLocked(meta, instance);

    // We can safely return the node that can be used by any thread, because
    // a DeviceNode never gets deleted.
    return node;
}

// DeviceNode *DeviceNode::getDeviceNode(const char *node_name) {
// }

bool DeviceNode::deviceNodeExists(ORB_ID id, const uint8_t instance) {
    if ((id == ORB_ID::INVALID) || (instance > ORB_MULTI_MAX_INSTANCES - 1)) {
        return false;
    }

    return _node_exist[instance][(uint8_t)id];
}

ssize_t DeviceNode::write(const void *data) {
    if (!data) {
        return 0;
    }

    if (!_data) {
        // rt_enter_critical();
        _data = (uint8_t *)rt_calloc(1, _meta->o_size * _queue_size);
        // rt_exit_critical();

        /* failed or could not allocate */
        if (_data) {
            return 0;
        }
    }

    /* Perform an atomic copy. */
    /* wrap-around happens after ~49 days, assuming a publisher rate of 1 kHz */
    unsigned generation = _generation.fetch_add(1);

    rt_enter_critical();
    rt_memcpy(_data + (_meta->o_size * (generation % _queue_size)), data, _meta->o_size);

    // 注册callback
    // for (auto item : _callbacks) {
    //     item->call();
    // }

    /* Mark at least one data has been published */
    _data_valid = true;

    rt_exit_critical();

    /* notify any poll waiters */
    // poll_notify(POLLIN);

    return _meta->o_size;
}

bool DeviceNode::read(void *dst, unsigned &generation) {
    return 0;
}

void DeviceNode::add_internal_subscriber() {
    rt_enter_critical();
    _subscriber_count++;
    rt_exit_critical();
}

void DeviceNode::remove_internal_subscriber() {
    rt_enter_critical();
    _subscriber_count--;
    rt_exit_critical();
}

int DeviceNode::update_queue_size(unsigned int queue_size)

{
    if (_queue_size == queue_size) {
        return 0;
    }

    // queue size is limited to 255 for the single reason that we use uint8 to store it
    if (_data || _queue_size > queue_size || queue_size > 255) {
        return -1;
    }

    _queue_size = round_pow_of_two_8(queue_size);
    return 0;
}

unsigned DeviceNode::get_initial_generation() {
    // rt_enter_critical();

    // If there any previous publications allow the subscriber to read them
    unsigned generation = _generation.load() - (_data_valid ? 1 : 0);

    // rt_exit_critical();

    return generation;
}

// bool DeviceNode::register_callback(uORB::SubscriptionCallback *callback_sub) {
//     if (callback_sub != nullptr) {
//         rt_enter_critical();

//         // prevent duplicate registrations
//         for (auto existing_callbacks : _callbacks) {
//             if (callback_sub == existing_callbacks) {
//                 rt_exit_critical();
//                 return true;
//             }
//         }

//         _callbacks.add(callback_sub);
//         rt_exit_critical();
//         return true;
//     }

//     return false;
// }

// void uORB::DeviceNode::unregister_callback(uORB::SubscriptionCallback *callback_sub) {
//     rt_enter_critical();
//     _callbacks.remove(callback_sub);
//     rt_exit_critical();
// }

} // namespace nextpilot::uORB
