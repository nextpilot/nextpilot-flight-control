/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __UORB_SUBSCRIPTION_H__
#define __UORB_SUBSCRIPTION_H__

#include <rtthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <rtdbg.h>
#include "uORB.h"

#ifdef __cplusplus
#include <pthread.h>
#include <IntrusiveList.hpp>
#include <LockGuard.hpp>
#ifdef PKG_USING_HRTIMER
#include <hrtimer.h>
#endif // PKG_USING_HRTIMER
#include <uORB/topics/uORBTopics.hpp>
#include <uORBDeviceNode.hpp>

#ifdef PKG_USING_WORKQUEUE
#include "WorkItem.hpp"
#endif // PKG_USING_WORKQUEUE

namespace nextpilot::uORB {

class DeviceNode;

#ifndef PKG_USING_HRTIMER
typedef uint64_t       hrt_abstime;
static inline uint64_t hrt_absolute_time() {
    return rt_tick_get() * 1000ULL;
}
static inline uint64_t hrt_elapsed_time(uint64_t *time) {
    return rt_tick_get() * 1000ULL - (*time);
}
#endif // PKG_USING_HRTIMER

class Subscription {
public:
    /**
     * Constructor
     *
     * @param id The uORB ORB_ID enum for the topic.
     * @param instance The instance for multi sub.
     */
    Subscription(ORB_ID id, uint8_t instance = 0) :
        _orb_id(id),
        _instance(instance) {
        subscribe();
    }

    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param instance The instance for multi sub.
     */
    Subscription(const orb_metadata *meta = nullptr, uint8_t instance = 0) :
        _orb_id((meta == nullptr) ? ORB_ID::INVALID : static_cast<ORB_ID>(meta->o_id)),
        _instance(instance) {
        subscribe();
    }

    // Copy constructor
    Subscription(const Subscription &other) :
        _orb_id(other._orb_id),
        _instance(other._instance) {
    }

    // Move constructor
    Subscription(const Subscription &&other) noexcept :
        _orb_id(other._orb_id),
        _instance(other._instance) {
    }

    // copy assignment
    Subscription &operator=(const Subscription &other) {
        unsubscribe();
        _orb_id   = other._orb_id;
        _instance = other._instance;
        return *this;
    }

    // move assignment
    Subscription &operator=(Subscription &&other) noexcept {
        unsubscribe();
        _orb_id   = other._orb_id;
        _instance = other._instance;
        return *this;
    }

    ~Subscription() {
        unsubscribe();
    }

    bool subscribe() {
        // check if already subscribed
        if (_node != nullptr) {
            return true;
        }

        if (_orb_id != ORB_ID::INVALID) {
            DeviceNode *node = DeviceNode::getDeviceNode(get_orb_meta(_orb_id), _instance);

            if (node) {
                node->add_internal_subscriber();
                _node            = node;
                _last_generation = node->get_initial_generation();
                return true;
            }
        }

        return false;
    }
    void unsubscribe() {
        if (_node != nullptr) {
            _node->remove_internal_subscriber();
        }

        _node            = nullptr;
        _last_generation = 0;
    }

    bool valid() const {
        return _node != nullptr;
    }

    bool advertised() {
        if (valid()) {
            return _node->is_advertised();
        }

        // try to initialize
        if (subscribe()) {
            // check again if valid
            if (valid()) {
                return _node->is_advertised();
            }
        }

        return false;
    }

    unsigned updates_available() {
        if (_node && _node->is_advertised()) {
            return _node->updates_available(_last_generation);
        }

        return 0;
    }

    /**
     * Check if there is a new update.
     */
    bool updated() {
        if (!valid()) {
            subscribe();
        }

        return valid() ? updates_available() : false;
    }

    /**
     * Update the struct
     * @param dst The uORB message struct we are updating.
     */
    bool update(void *dst) {
        if (advertised() && updates_available()) {
            return _node->read(dst, _last_generation);
        }

        return false;
    }

    /**
     * Copy the struct
     * @param dst The uORB message struct we are updating.
     */
    bool copy(void *dst) {
        if (advertised()) {
            return _node->read(dst, _last_generation);
        }

        return false;
    }

    bool ChangeInstance(uint8_t instance) {
        return change_instance(instance);
    }

    /**
     * Change subscription instance
     * @param instance The new multi-Subscription instance
     */
    bool change_instance(uint8_t instance) {
        if (instance != _instance) {
            if (DeviceNode::deviceNodeExists(_orb_id, _instance)) {
                // if desired new instance exists, unsubscribe from current
                unsubscribe();
                _instance = instance;
                subscribe();
                return true;
            }

        } else {
            // already on desired index
            return true;
        }

        return false;
    }

    uint8_t get_instance() const {
        return _instance;
    }

    unsigned get_last_generation() const {
        return _last_generation;
    }

    orb_id_t get_topic() const {
        return get_orb_meta(_orb_id);
    }

    ORB_ID orb_id() const {
        return _orb_id;
    }

protected:
    // friend class SubscriptionCallback;
    // friend class SubscriptionCallbackWorkItem;

    DeviceNode *get_node() {
        return _node;
    }

    ORB_ID      _orb_id          = ORB_ID::INVALID;
    DeviceNode *_node            = nullptr;
    unsigned    _last_generation = 0;
    uint8_t     _instance        = 0;
};

class SubscriptionInterval : public Subscription {
public:
    /**
     * Constructor
     *
     * @param id The uORB ORB_ID enum for the topic.
     * @param interval The requested maximum update interval in microseconds.
     * @param instance The instance for multi sub.
     */
    SubscriptionInterval(ORB_ID id, uint32_t interval_us = 0, uint8_t instance = 0) :
        Subscription(id, instance),
        _interval_us(interval_us) {
    }

    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param interval The requested maximum update interval in microseconds.
     * @param instance The instance for multi sub.
     */
    SubscriptionInterval(const orb_metadata *meta, uint32_t interval_us = 0, uint8_t instance = 0) :
        Subscription{meta, instance},
        _interval_us(interval_us) {
    }

    SubscriptionInterval() :
        Subscription{nullptr} {
    }

    ~SubscriptionInterval() = default;

    /**
     * Check if there is a new update.
     * */
    bool updated() {
        if (advertised() && hrt_elapsed_time(&_last_update) >= _interval_us) {
            return Subscription::updated();
        }

        return false;
    }

    /**
     * Copy the struct if updated.
     * @param dst The destination pointer where the struct will be copied.
     * @return true only if topic was updated and copied successfully.
     */
    bool update(void *dst) {
        if (updated()) {
            return copy(dst);
        }

        return false;
    }

    /**
     * Copy the struct
     * @param dst The destination pointer where the struct will be copied.
     * @return true only if topic was copied successfully.
     */
    bool copy(void *dst) {
        if (Subscription::copy(dst)) {
            const hrt_abstime now = hrt_absolute_time();
            // shift last update time forward, but don't let it get further behind than the interval
            // _last_update = math::constrain(_last_update + _interval_us, now - _interval_us, now);
            if (_last_update + _interval_us < now - _interval_us) {
                _last_update = now - _interval_us;
            } else if (_last_update + _interval_us > now) {
                _last_update = now;
            } else {
                _last_update = _last_update + _interval_us;
            }
            return true;
        }

        return false;
    }

    uint32_t get_interval_us() const {
        return _interval_us;
    }

    /**
     * Set the interval in microseconds
     * @param interval The interval in microseconds.
     */
    void set_interval_us(uint32_t interval) {
        _interval_us = interval;
    }

    /**
     * Set the interval in milliseconds
     * @param interval The interval in milliseconds.
     */
    void set_interval_ms(uint32_t interval) {
        _interval_us = interval * 1000;
    }

    /**
     * Set the last data update
     * @param t should be in range [now, now - _interval_us]
     */
    void set_last_update(hrt_abstime t) {
        _last_update = t;
    }

protected:
    uint64_t _last_update{0}; // last update in microseconds
    uint32_t _interval_us{0}; // maximum update interval in microseconds
};

template <class T>
class SubscriptionData : public Subscription {
public:
    /**
     * Constructor
     *
     * @param id The uORB metadata ORB_ID enum for the topic.
     * @param instance The instance for multi sub.
     */
    SubscriptionData(ORB_ID id, uint8_t instance = 0) :
        Subscription(id, instance) {
        copy(&_data);
    }

    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param instance The instance for multi sub.
     */
    SubscriptionData(const orb_metadata *meta, uint8_t instance = 0) :
        Subscription(meta, instance) {
        copy(&_data);
    }

    ~SubscriptionData() = default;

    // no copy, assignment, move, move assignment
    SubscriptionData(const SubscriptionData &)            = delete;
    SubscriptionData &operator=(const SubscriptionData &) = delete;
    SubscriptionData(SubscriptionData &&)                 = delete;
    SubscriptionData &operator=(SubscriptionData &&)      = delete;

    // update the embedded struct.
    bool update() {
        return Subscription::update((void *)(&_data));
    }

    const T &get() const {
        return _data;
    }

private:
    T _data{};
};

/**
 * An array of uORB::Subscriptions of the same topic
 */
template <typename T, uint8_t SIZE = ORB_MULTI_MAX_INSTANCES>
class SubscriptionMultiArray {
public:
    static_assert(SIZE <= ORB_MULTI_MAX_INSTANCES, "size must be <= uORB max instances");

    static constexpr uint8_t size() {
        return SIZE;
    }

    /**
     * Constructor
     *
     * @param id The uORB ORB_ID enum for the topic.
     */
    explicit SubscriptionMultiArray(ORB_ID id) {
        for (uint8_t i = 0; i < SIZE; i++) {
            _subscriptions[i] = Subscription{id, i};
            _subscriptions[i].subscribe();
        }
    }

    ~SubscriptionMultiArray() = default;

    Subscription &operator[](int i) {
        return _subscriptions[i];
    }
    const Subscription &operator[](int i) const {
        return _subscriptions[i];
    }

    Subscription *begin() {
        return _subscriptions;
    }
    Subscription *end() {
        return _subscriptions + SIZE;
    }

    const Subscription *begin() const {
        return _subscriptions;
    }
    const Subscription *end() const {
        return _subscriptions + SIZE;
    }

    // true if any instance is advertised
    bool advertised() {
        for (auto &s : _subscriptions) {
            if (s.advertised()) {
                return true;
            }
        }

        return false;
    }

    // return the number of instances currently advertised
    uint8_t advertised_count() {
        uint8_t count = 0;

        for (auto &s : _subscriptions) {
            if (s.advertised()) {
                count++;
            }
        }

        return count;
    }

    // true if any instance is updated
    bool updated() {
        for (auto &s : _subscriptions) {
            if (s.updated()) {
                return true;
            }
        }

        return false;
    }

private:
    Subscription _subscriptions[SIZE];
};

// Subscription wrapper class with callbacks on new publications
class SubscriptionCallback : public SubscriptionInterval, public ListNode<SubscriptionCallback *> {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param interval_us The requested maximum update interval in microseconds.
     * @param instance The instance for multi sub.
     */
    SubscriptionCallback(const orb_metadata *meta, uint32_t interval_us = 0, uint8_t instance = 0) :
        SubscriptionInterval(meta, interval_us, instance) {
    }

    virtual ~SubscriptionCallback() {
        unregister_callback();
    };

    bool registerCallback() {
        return register_callback();
    }

    void unregisterCallback() {
        unregister_callback();
    }

    bool register_callback() {
        if (!_registered) {
            if (get_node() && get_node()->register_callback(this)) {
                // registered
                _registered = true;

            } else if (subscribe() && get_node()->register_callback(this)) {
                _registered = true;

            } else {
                // force topic creation by subscribing with old API
                DeviceNode *node = new DeviceNode(get_topic(), get_instance());

                // try to register callback again
                if (node && subscribe()) {
                    if (node->register_callback(this)) {
                        _registered = true;
                    }
                }
            }
        }

        return _registered;
    }

    virtual void unregister_callback() {
        if (get_node()) {
            get_node()->unregister_callback(this);
        }

        _registered = false;
    }

    bool ChangeInstance(uint8_t instance) {
        return change_instance(instance);
    }

    /**
     * Change subscription instance
     * @param instance The new multi-Subscription instance
     */
    bool change_instance(uint8_t instance) {
        bool ret = false;

        if (instance != get_instance()) {
            const bool registered = _registered;

            if (registered) {
                unregister_callback();
            }

            if (SubscriptionInterval::change_instance(instance)) {
                ret = true;
            }

            if (registered) {
                register_callback();
            }

        } else {
            // already on desired index
            return true;
        }

        return ret;
    }

    virtual void call() = 0;

    bool registered() const {
        return _registered;
    }

protected:
    bool _registered{false};
};

// Subscription with callback that schedules a WorkItem
class SubscriptionPolling : public SubscriptionCallback {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param interval_us The requested maximum update interval in microseconds.
     * @param instance The instance for multi sub.
     */
    SubscriptionPolling(const orb_metadata *meta, uint32_t interval_us = 0, uint8_t instance = 0) :
        SubscriptionCallback(meta, interval_us, instance) {
    }

    virtual ~SubscriptionPolling() {
        unregister_callback();
    }

    void call() override {
        if (_cv) {
            // signal immediately if no interval, otherwise only if interval has elapsed
            if ((_interval_us == 0) || (hrt_elapsed_time(&_last_update) >= _interval_us)) {
                pthread_cond_signal(_cv);
            }
        }
    }

    bool register_callback(pthread_cond_t *cv) {
        _cv = cv;
        return SubscriptionCallback::register_callback();
    }

    void unregister_callback() override {
        SubscriptionCallback::unregister_callback();
        _cv = nullptr;
    }

private:
    pthread_cond_t *_cv = nullptr;
};

template <typename T>
class SubscriptionBlocking : public SubscriptionCallback {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param interval_us The requested maximum update interval in microseconds.
     * @param instance The instance for multi sub.
     */
    SubscriptionBlocking(const orb_metadata *meta, uint32_t interval_us = 0, uint8_t instance = 0) :
        SubscriptionCallback(meta, interval_us, instance) {
        // pthread_mutexattr_init
        pthread_mutexattr_t attr;
        int                 ret_attr_init = pthread_mutexattr_init(&attr);

        if (ret_attr_init != 0) {
            LOG_E("pthread_mutexattr_init failed, status=%d", ret_attr_init);
        }

#if defined(PTHREAD_PRIO_NONE)
        // pthread_mutexattr_settype
        //  PTHREAD_PRIO_NONE not available on cygwin
        int ret_mutexattr_settype = pthread_mutexattr_settype(&attr, PTHREAD_PRIO_NONE);

        if (ret_mutexattr_settype != 0) {
            LOG_E("pthread_mutexattr_settype failed, status=%d", ret_mutexattr_settype);
        }

#endif // PTHREAD_PRIO_NONE

        // pthread_mutex_init
        int ret_mutex_init = pthread_mutex_init(&_mutex, &attr);

        if (ret_mutex_init != 0) {
            LOG_E("pthread_mutex_init failed, status=%d", ret_mutex_init);
        }
    }

    virtual ~SubscriptionBlocking() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cv);
    }

    void call() override {
        // signal immediately if no interval, otherwise only if interval has elapsed
        if ((_interval_us == 0) || (hrt_elapsed_time(&_last_update) >= _interval_us)) {
            pthread_cond_signal(&_cv);
        }
    }

    bool updatedBlocking(uint32_t timeout_us = 0) {
        return updated_blocking(timeout_us);
    }

    /**
     * Block until updated.
     * @param timeout_us The requested timeout in microseconds, or 0 to wait indefinitely.
     *
     * @return true only if topic was updated
     */
    bool updated_blocking(uint32_t timeout_us = 0) {
        if (!_registered) {
            register_callback();
        }

        if (updated()) {
            // return immediately if updated
            return true;

        } else {
            // otherwise wait

            LockGuard lg{_mutex};

            if (timeout_us == 0) {
                // wait with no timeout
                if (pthread_cond_wait(&_cv, &_mutex) == 0) {
                    return updated();
                }

            } else {
                // otherwise wait with timeout based on interval

                // Calculate an absolute time in the future
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                uint64_t                  nsecs   = ts.tv_nsec + (timeout_us * 1000);
                static constexpr unsigned billion = (1000 * 1000 * 1000);
                ts.tv_sec += nsecs / billion;
                nsecs -= (nsecs / billion) * billion;
                ts.tv_nsec = nsecs;

                if (pthread_cond_timedwait(&_cv, &_mutex, &ts) == 0) {
                    return updated();
                }
            }
        }

        return false;
    }

    /**
     * Copy the struct if updated.
     * @param data The data reference where the struct will be copied.
     * @param timeout_us The requested timeout in microseconds, or 0 to wait indefinitely.
     *
     * @return true only if topic was updated and copied successfully.
     */
    bool updateBlocking(T &data, uint32_t timeout_us = 0) {
        if (updated_blocking(timeout_us)) {
            return copy(&data);
        }

        return false;
    }

private:
    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  _cv    = PTHREAD_COND_INITIALIZER;
};

#ifdef PKG_USING_WORKQUEUE

// Subscription with callback that schedules a WorkItem
class SubscriptionCallbackWorkItem : public SubscriptionCallback {
public:
    /**
     * Constructor
     *
     * @param work_item The WorkItem that will be scheduled immediately on new publications.
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param instance The instance for multi sub.
     */
    SubscriptionCallbackWorkItem(nextpilot::WorkItem *work_item, const orb_metadata *meta, uint8_t instance = 0) :
        SubscriptionCallback(meta, 0, instance), // interval 0
        _work_item(work_item) {
    }

    virtual ~SubscriptionCallbackWorkItem() = default;

    void call() override {
        // schedule immediately if updated (queue depth or subscription interval)
        // if ((_required_updates == 0) || (uorb_device_updates_available(get_node(), get_last_generation()) >= _required_updates)) {
        //     if (updated()) {
        //         _work_item->ScheduleNow();
        //     }
        // }
    }

    /**
     * Optionally limit callback until more samples are available.
     *
     * @param required_updates Number of queued updates required before a callback can be called.
     */
    void set_required_updates(uint8_t required_updates) {
        // TODO: constrain to queue depth?
        _required_updates = required_updates;
    }

private:
    nextpilot::WorkItem *_work_item;

    uint8_t _required_updates{0};
};

#endif // PKG_USING_WORKQUEUE

} // namespace nextpilot::uORB
#endif //__cplusplus

#endif // __UORB_SUBSCRIPTION_H__
