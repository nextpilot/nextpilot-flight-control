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
#include "uorb_device_node.h"
#include "uORB.h"

#define ORB_ID_INVALID UINT16_MAX

#define __EXPORT

typedef uint16_t ORB_ID;

struct uorb_subscription {
    uint16_t      orb_id;
    uorb_device_t node;
    uint32_t      last_generation;
    uint8_t       instance;
};

typedef struct uorb_subscription  orb_subval_t;
typedef struct uorb_subscription *orb_subptr_t;

rt_err_t orb_exists(const struct orb_metadata *meta, int instance);
int      orb_group_count(const struct orb_metadata *meta);
bool     orb_data_copy(uorb_device_t node, void *dst, uint32_t *generation, bool only_if_updated);

orb_subval_t orb_subscribe_multi(const struct orb_metadata *meta, uint8_t instance);
orb_subval_t orb_subscribe(const struct orb_metadata *meta);
rt_err_t     orb_unsubscribe(orb_subptr_t sub);
rt_err_t     orb_check(orb_subptr_t sub, bool *updated);
rt_err_t     orb_copy(const struct orb_metadata *meta, orb_subptr_t sub, void *buffer);
rt_err_t     orb_update(orb_subptr_t sub, void *data);
rt_err_t     orb_poll(orb_subptr_t sub, int timeout_us);
rt_err_t     orb_change_instance(orb_subptr_t sub, uint8_t instance);

int orb_set_interval(orb_subptr_t sub, unsigned interval);
int orb_get_interval(orb_subptr_t sub, unsigned *interval);
// rt_err_t orb_register_callback(uorb_device_t node);
// rt_err_t orb_unregister_callback(uorb_device_t node);

#ifdef __cplusplus
#include <IntrusiveList.hpp>
#include <pthread.h>
#include <LockGuard.hpp>

namespace nextpilot::uORB {

class Subscription {
public:
    /**
     * Constructor
     *
     * @param id The uORB ORB_ID enum for the topic.
     * @param instance The instance for multi sub.
     */
    Subscription(ORB_ID id, uint8_t instance = 0) :
        _handle{
            .orb_id   = id,
            .instance = instance,
        } {
        subscribe();
    }

    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param instance The instance for multi sub.
     */
    Subscription(const orb_metadata *meta = nullptr, uint8_t instance = 0) :
        _handle{
            .orb_id = 0,
            // ((meta == nullptr) ? ORB_ID_INVALID : static_cast<ORB_ID>(meta->o_id)),
            .instance = (instance),
        } {
        subscribe();
    }

    // Copy constructor
    Subscription(const Subscription &other) :
        _handle{
            .orb_id   = (other._handle.orb_id),
            .instance = (other._handle.instance),
        } {
    }

    // Move constructor
    Subscription(const Subscription &&other) noexcept :
        _handle{
            .orb_id   = (other._handle.orb_id),
            .instance = (other._handle.instance),
        } {
    }

    // copy assignment
    Subscription &operator=(const Subscription &other) {
        unsubscribe();
        _handle.orb_id   = other._handle.orb_id;
        _handle.instance = other._handle.instance;
        return *this;
    }

    // move assignment
    Subscription &operator=(Subscription &&other) noexcept {
        unsubscribe();
        _handle.orb_id   = other._handle.orb_id;
        _handle.instance = other._handle.instance;
        return *this;
    }

    ~Subscription() {
        unsubscribe();
    }

    bool subscribe() {
        // check if already subscribed
        if (_handle.node != nullptr) {
            return true;
        }

        if (_handle.orb_id != ORB_ID_INVALID) {
            unsigned      initial_generation;
            uorb_device_t node = nullptr;
            node               = uorb_device_add_internal_subscriber(_handle.orb_id, _handle.instance, &initial_generation);

            if (node) {
                _handle.node            = node;
                _handle.last_generation = initial_generation;
                return true;
            }
        }

        return false;
    }
    void unsubscribe() {
        if (_handle.node != nullptr) {
            uorb_device_remove_internal_subscriber(_handle.node);
        }

        _handle.node            = nullptr;
        _handle.last_generation = 0;
    }

    bool valid() const {
        return _handle.node != nullptr;
    }

    bool advertised() {
        if (valid()) {
            return uorb_device_node_advertised(_handle.node);
        }

        // try to initialize
        if (subscribe()) {
            // check again if valid
            if (valid()) {
                return uorb_device_node_advertised(_handle.node);
            }
        }

        return false;
    }

    /**
     * Check if there is a new update.
     */
    bool updated() {
        if (!valid()) {
            subscribe();
        }

        return valid() ? uorb_device_updates_available(_handle.node, _handle.last_generation) : false;
    }

    /**
     * Update the struct
     * @param dst The uORB message struct we are updating.
     */
    bool update(void *dst) {
        if (!valid()) {
            subscribe();
        }
        return valid() ? orb_data_copy(_handle.node, dst, &_handle.last_generation, true) : false;
    }

    /**
     * Copy the struct
     * @param dst The uORB message struct we are updating.
     */
    bool copy(void *dst) {
        if (!valid()) {
            subscribe();
        }
        return valid() ? orb_data_copy(_handle.node, dst, &_handle.last_generation, false) : false;
    }

    /**
     * Change subscription instance
     * @param instance The new multi-Subscription instance
     */
    bool change_instance(uint8_t instance) {
        if (instance != _handle.instance) {
            if (uorb_device_node_exist(_handle.orb_id, _handle.instance)) {
                // if desired new instance exists, unsubscribe from current
                unsubscribe();
                _handle.instance = instance;
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
        return _handle.instance;
    }

    unsigned get_last_generation() const {
        return _handle.last_generation;
    }

    orb_id_t get_topic() const {
        return get_orb_meta(_handle.orb_id);
    }

    ORB_ID orb_id() const {
        return _handle.orb_id;
    }

protected:
    // friend class SubscriptionCallback;
    // friend class SubscriptionCallbackWorkItem;

    void *get_node() {
        return _handle.node;
    }

    orb_subval_t _handle{
        .orb_id          = ORB_ID_INVALID,
        .node            = nullptr,
        .last_generation = 0,
        .instance        = 0,
    };
};

#ifndef PKG_USING_HRTIMER
typedef uint64_t       hrt_abstime;
static inline uint64_t hrt_absolute_time() {
    return rt_tick_get() * 1000ULL;
}
static inline uint64_t hrt_elapsed_time(uint64_t *time) {
    return rt_tick_get() * 1000ULL - (*time);
}
#endif // PKG_USING_HRTIMER
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

    bool register_callback() {
        if (!_registered) {
            if (get_node() /*&& Manager::register_callback(get_node(), this)*/) {
                // registered
                _registered = true;

            } else {
                // force topic creation by subscribing with old API
                orb_subval_t fd = orb_subscribe_multi(get_topic(), get_instance());

                // try to register callback again
                if (subscribe()) {
                    if (get_node() /*&& Manager::register_callback(get_node(), this)*/) {
                        _registered = true;
                    }
                }

                orb_unsubscribe(&fd);
            }
        }

        return _registered;
    }

    void unregister_callback() {
        if (get_node()) {
            /* Manager::unregister_callback(get_node(), this);*/
        }

        _registered = false;
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

} // namespace nextpilot::uORB
#endif //__cplusplus

#endif // __UORB_SUBSCRIPTION_H__
