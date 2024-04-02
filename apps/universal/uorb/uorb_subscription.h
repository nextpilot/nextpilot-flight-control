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
#include "uORB.h"

#define __EXPORT

typedef uint16_t ORB_ID;

struct uorb_subscription {
    uint16_t _orb_id;
    void    *_node;
    uint32_t _last_generation;
    uint8_t  _instance;
};

typedef struct uorb_subscription  orb_subval_t;
typedef struct uorb_subscription *orb_subptr_t;

rt_err_t orb_exists(const struct orb_metadata *meta, int instance);
int      orb_group_count(const struct orb_metadata *meta);

orb_subval_t orb_subscribe_multi(const struct orb_metadata *meta, uint8_t instance);
orb_subval_t orb_subscribe(const struct orb_metadata *meta);
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

class Subscription : public uorb_subscription {
public:
    /**
     * Constructor
     *
     * @param id The uORB ORB_ID enum for the topic.
     * @param instance The instance for multi sub.
     */
    Subscription(uint16_t id, uint8_t instance = 0) {
        _orb_id   = id;
        _instance = instance;
        subscribe();
    }

    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     * @param instance The instance for multi sub.
     */
    Subscription(const orb_metadata *meta = nullptr, uint8_t instance = 0) {
#define ORB_ID_INVALID UINT32_MAX
        _orb_id   = ((meta == nullptr) ? ORB_ID_INVALID : static_cast<ORB_ID>(meta->o_id));
        _instance = (instance);
        subscribe();
    }

    // Copy constructor
    Subscription(const Subscription &other) {
        _orb_id   = (other._orb_id);
        _instance = (other._instance);
    }

    // Move constructor
    Subscription(const Subscription &&other) noexcept {
        _orb_id   = (other._orb_id);
        _instance = (other._instance);
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

        if (_orb_id != ORB_ID_INVALID && uORB::Manager::get_instance()) {
            unsigned initial_generation;
            void    *node = nullptr;
            // uORB::Manager::orb_add_internal_subscriber(_orb_id, _instance, &initial_generation);

            if (node) {
                _node            = node;
                _last_generation = initial_generation;
                return true;
            }
        }

        return false;
    }
    void unsubscribe() {
        if (_node != nullptr) {
            // uORB::Manager::orb_remove_internal_subscriber(_node);
        }

        _node            = nullptr;
        _last_generation = 0;
    }

    bool valid() const {
        return _node != nullptr;
    }

    bool advertised() {
        if (valid()) {
            return Manager::is_advertised(_node);
        }

        // try to initialize
        if (subscribe()) {
            // check again if valid
            if (valid()) {
                return Manager::is_advertised(_node);
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

        // return valid() ? Manager::updates_available(_node, _last_generation) : false;
        return true;
    }

    /**
     * Update the struct
     * @param dst The uORB message struct we are updating.
     */
    bool update(void *dst) {
        if (!valid()) {
            subscribe();
        }
        return true;
        // return valid() ? Manager::orb_data_copy(_node, dst, _last_generation, true) : false;
    }

    /**
     * Copy the struct
     * @param dst The uORB message struct we are updating.
     */
    bool copy(void *dst) {
        if (!valid()) {
            subscribe();
        }
        return true;
        // return valid() ? Manager::orb_data_copy(_node, dst, _last_generation, false) : false;
    }

    /**
     * Change subscription instance
     * @param instance The new multi-Subscription instance
     */
    bool change_instance(uint8_t instance) {
        if (instance != _instance) {
            if (uORB::Manager::orb_device_node_exists(_orb_id, _instance)) {
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

    void *get_node() {
        return _node;
    }
};

class SubscriptionInverval : public Subscription {
    /**
     * Constructor
     *
     * @param id The uORB ORB_ID enum for the topic.
     * @param interval The requested maximum update interval in microseconds.
     * @param instance The instance for multi sub.
     */
    SubscriptionInterval(ORB_ID id, uint32_t interval_us = 0, uint8_t instance = 0) :
        Subscription{id, instance},
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
            _last_update = math::constrain(_last_update + _interval_us, now - _interval_us, now);
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

} // namespace nextpilot::uORB
#endif //__cplusplus

#endif // __UORB_SUBSCRIPTION_H__
