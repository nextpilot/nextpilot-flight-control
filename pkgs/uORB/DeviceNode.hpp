/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __UORB_DEVICE_NODE_H__
#define __UORB_DEVICE_NODE_H__

#include <rtthread.h>
#include <stdint.h>

#ifdef __cplusplus
#   include <atomic.hpp>
#   include <IntrusiveSortedList.hpp>
#   include <IntrusiveList.hpp>
#   include <atomic_bitset.hpp>
#   include <uORB.h>
#   include <uORB/topics/uORBTopics.hpp>

namespace nextpilot::uORB {

// Determine the data range
static inline bool is_in_range(unsigned left, unsigned value, unsigned right) {
    if (right > left) {
        return (left <= value) && (value <= right);

    } else { // Maybe the data overflowed and a wraparound occurred
        return (left <= value) || (value <= right);
    }
}

// round up to nearest power of two
// Such as 0 => 1, 1 => 1, 2 => 2 ,3 => 4, 10 => 16, 60 => 64, 65...255 => 128
// Note: When the input value > 128, the output is always 128
static inline uint8_t round_pow_of_two_8(uint8_t n) {
    if (n == 0) {
        return 1;
    }

    // Avoid is already a power of 2
    uint8_t value = n - 1;

    // Fill 1
    value |= value >> 1U;
    value |= value >> 2U;
    value |= value >> 4U;

    // Unable to round-up, take the value of round-down
    if (value == UINT8_MAX) {
        value >>= 1U;
    }

    return value + 1;
}

class DeviceNode;
class SubscriptionCallback;

class DeviceNode : public IntrusiveSortedListNode<DeviceNode *> {
public:
    DeviceNode(const struct orb_metadata *meta, const uint8_t instance, uint8_t queue_size = 1) :
        _meta(meta),
        _instance(instance),
        _queue_size(round_pow_of_two_8(queue_size)) {
        _node_list.add(this);
        _node_exist[_instance].set(_meta->o_id, true);
    }

    virtual ~DeviceNode() {
        if (_subscriber_count > 0) {
            // LOG_E("delete %s fail, as subscriber count = %d", _meta->o_name, _subscriber_count)
            return;
        }

        _node_list.remove(this);
        if (_data) {
            rt_free(_data);
        }
    }

    // no copy, assignment, move, move assignment
    DeviceNode(const DeviceNode &)            = delete;
    DeviceNode &operator=(const DeviceNode &) = delete;
    DeviceNode(DeviceNode &&)                 = delete;
    DeviceNode &operator=(DeviceNode &&)      = delete;

    bool operator<=(const DeviceNode &rhs) const {
        // return (rt_strcmp(get_devname(), rhs.get_devname()) <= 0);
        return (rt_strcmp(_meta->o_name, rhs._meta->o_name) <= 0);
    }

    /**
     * Method to publish a data to this node.
     */
    ssize_t write(const void *buffer);

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
    bool read(void *dst, unsigned &generation);

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
    int update_queue_size(unsigned int queue_size);

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
        return _generation.load() - generation;
    }

    /**
     * Return the initial generation to the subscriber
     * @return The initial generation.
     */
    unsigned get_initial_generation();

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

    // add item to list of work items to schedule on node update
    bool register_callback(SubscriptionCallback *callback_sub);

    // remove item from list of work items
    void unregister_callback(SubscriptionCallback *callback_sub);

    /**
     * Print statistics
     * @param max_topic_length max topic name length for printing
     * @return true if printed something, false otherwise
     */
    bool print_statistics(int max_topic_length);

private:
    const orb_metadata          *_meta;              /**< object metadata information */
    uint8_t                     *_data{nullptr};     /**< allocated object buffer */
    bool                         _data_valid{false}; /**< At least one valid data */
    atomic<unsigned>             _generation{0};     /**< object generation count */
    List<SubscriptionCallback *> _callbacks;
    const uint8_t                _instance;          /**< orb multi instance identifier */
    bool                         _advertised{false}; /**< has ever been advertised (not necessarily published data yet) */
    uint8_t                      _queue_size;        /**< maximum number of elements in the queue */
    int8_t                       _subscriber_count{0};

public:
    static IntrusiveSortedList<DeviceNode *> _node_list;
    static AtomicBitset<ORB_TOPICS_COUNT>    _node_exist[ORB_MULTI_MAX_INSTANCES];

    static DeviceNode *advertise(const struct orb_metadata *meta, const void *data, int *instance, uint8_t queue_size = 1);
    static int         unadvertise(DeviceNode *handle);

    static DeviceNode *getDeviceNodeLocked(const struct orb_metadata *meta, const uint8_t instance);
    static DeviceNode *getDeviceNode(const struct orb_metadata *meta, const uint8_t instance);
    // static DeviceNode *getDeviceNode(const char *node_name);
    static bool deviceNodeExists(ORB_ID id, const uint8_t instance);
    // 发布主题
    static int publish(const orb_metadata *meta, orb_advert_t handle, const void *data);
};

} // namespace nextpilot::uORB

#endif //__cplusplus

#endif // __UORB_DEVICE_NODE_H__
