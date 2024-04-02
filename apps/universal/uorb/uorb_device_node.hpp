/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <rtthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "atomic.h"
#include "uorb_device_node.h"

// #ifdef __cplusplus
namespace nextpilot::uORB {
class DeviceNode : public uorb_device_node {
    DeviceNode(const struct orb_metadata *meta, const uint8_t instance, const char *path, uint8_t queue_size = 1) {
        _meta             = meta;
        _data             = nullptr;
        _data_valid       = false;
        _generation       = 0;
        _instance         = instance;
        _advertised       = false;
        _queue_size       = round_pow_of_two_8(queue_size);
        _subscriber_count = 0;

        // 标记主题已经创建
        uorb_device_mark_exist(meta->o_id, instance);
        // 将主题节点添加到链表
        uorb_device_add_node(this);
    }

    virtual ~DeviceNode() {
        // 在还有订阅者的时候，不允许析构
        if (_subscriber_count > 0) {
            // LOG_E("can't destroy %s", _meta->name);
            return;
        }

        // 将节点从链表移除
        uorb_device_remove_node(this);
        // 标记主题不存在
        uorb_device_clear_exist(_meta->o_id, _instance);

        // 释放data指针
        if (_data) {
            void *data = _data;
            _data      = NULL;
            rt_free(data);
        }

        _data_valid       = false;
        _generation       = 0;
        _instance         = 0;
        _advertised       = false;
        _queue_size       = 0;
        _subscriber_count = 0;
    }

    // no copy, assignment, move, move assignment
    DeviceNode(const DeviceNode &)            = delete;
    DeviceNode &operator=(const DeviceNode &) = delete;
    DeviceNode(DeviceNode &&)                 = delete;
    DeviceNode &operator=(DeviceNode &&)      = delete;

    // bool operator<=(const DeviceNode &rhs) const {
    //     return (rt_strcmp(get_devname(), rhs.get_devname()) <= 0);
    // }

    /**
     * Method to publish a data to this node.
     */
    bool publish(const void *buffer) {
        if (!buffer) {
            return false;
        }

        // 动态分配data空间
        if (!_data) {
            // rt_enter_critical();
            const size_t data_size = _meta->o_size * _queue_size;
            _data                  = (uint8_t *)rt_calloc(1, data_size);
            // rt_exit_critical();

            /* failed or could not allocate */
            if (!_data) {
                return -1;
            }
        }

        /* Perform an atomic copy. */
        rt_enter_critical();
        /* wrap-around happens after ~49 days, assuming a publisher rate of 1 kHz */
        unsigned generation = rt_atomic_add(&_generation, 1L); //.fetch_add(1);

        rt_memcpy(_data + (_meta->o_size * (generation % _queue_size)), buffer, _meta->o_size);

        // 注册callback
        // for (auto item : _callbacks) {
        //     item->call();
        // }

        /* Mark at least one data has been published */
        _data_valid = true;

        rt_exit_critical();

        /* notify any poll waiters */
        // poll_notify(POLLIN);

        return true;
    }

    bool unadvertise(orb_advert_t handle) {
        return false;
    }

    /**
     * Add the subscriber to the node's list of subscriber.  If there is
     * remote proxy to which this subscription needs to be sent, it will
     * done via uORBCommunicator::IChannel interface.
     * @param sd
     *   the subscriber to be added.
     */
    void add_internal_subscriber();

    /**
     * Removes the subscriber from the list.  Also notifies the remote
     * if there a uORBCommunicator::IChannel instance.
     * @param sd
     *   the Subscriber to be removed.
     */
    void remove_internal_subscriber();

    /**
     * Return true if this topic has been advertised.
     *
     * This is used in the case of multi_pub/sub to check if it's valid to advertise
     * and publish to this node or if another node should be tried. */
    bool is_advertised() const {
        return _advertised;
    }

    void mark_as_advertised() {
        _advertised = true;
    }

    /**
     * Try to change the size of the queue. This can only be done as long as nobody published yet.
     * This is the case, for example when orb_subscribe was called before an orb_advertise.
     * The queue size can only be increased.
     * @param queue_size new size of the queue
     * @return 0 if queue size successfully set
     */
    int update_queue_size(unsigned int queue_size) {
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

    uint8_t get_queue_size() const {
        return _queue_size;
    }

    int8_t subscriber_count() const {
        return _subscriber_count;
    }

    /**
     * Returns the number of updated data relative to the parameter 'generation'
     * We can get the correct value regardless of wrap-around or not.
     * @param generation The generation of subscriber
     */
    unsigned updates_available(unsigned generation) const {
        return rt_atomic_load(&_generation) - generation;
    }

    /**
     * Return the initial generation to the subscriber
     * @return The initial generation.
     */
    unsigned get_initial_generation() {
        rt_enter_critical();
        // If there any previous publications allow the subscriber to read them
        unsigned generation = rt_atomic_load(&_generation) - (_data_valid ? 1 : 0);

        rt_exit_critical();
        return generation;
    }

    const orb_metadata *get_meta() const {
        return _meta;
    }

    ORB_ID id() const {
        return static_cast<ORB_ID>(_meta->o_id);
    }

    const char *get_name() const {
        return _meta->o_name;
    }

    uint8_t get_instance() const {
        return _instance;
    }

    /**
     * Copies data and the corresponding generation
     * from a node to the buffer provided.
     *
     * @param dst
     *   The buffer into which the data is copied.
     * @param generation
     *   The generation that was copied.
     * @return bool
     *   Returns true if the data was copied.
     */
    bool copy(void *dst, unsigned &generation) {
        if ((dst != nullptr) && (_data != nullptr)) {
            if (_queue_size == 1) {
                rt_enter_critical();
                rt_memcpy(dst, _data, _meta->o_size);
                generation = rt_atomic_load(&_generation);
                rt_exit_critical();
                return true;
            } else {
                rt_enter_critical();
                const unsigned current_generation = rt_atomic_load(&_generation);

                if (current_generation == generation) {
                    /* The subscriber already read the latest message, but nothing new was published yet.
                     * Return the previous message
                     */
                    --generation;
                }

                // Compatible with normal and overflow conditions
                if (!is_in_range(current_generation - _queue_size, generation, current_generation - 1)) {
                    // Reader is too far behind: some messages are lost
                    generation = current_generation - _queue_size;
                }

                rt_memcpy(dst, _data + (_meta->o_size * (generation % _queue_size)), _meta->o_size);
                rt_exit_critical();

                ++generation;

                return true;
            }
        }

        return false;
    }

    // add item to list of work items to schedule on node update
    bool register_callback(/*SubscriptionCallback *callback_sub*/) {
        return true;
    }

    // remove item from list of work items
    void unregister_callback(/*SubscriptionCallback *callback_sub*/) {
        return;
    }
};

} // namespace nextpilot::uORB

// #endif //__cplusplus
