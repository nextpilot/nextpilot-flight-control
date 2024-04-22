/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __UORB_PUBLICATION_H__
#define __UORB_PUBLICATION_H__

#include <stdint.h>
#include <uORB.h>

uint8_t orb_get_instance(const orb_advert_t node);
uint8_t orb_get_queue_size(const orb_advert_t node);

// orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance, unsigned int queue_size);
// orb_advert_t orb_advertise_queue(const struct orb_metadata *meta, const void *data, unsigned int queue_size);
// int          orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *node, const void *data, int *instance);

#ifdef __cplusplus
#include <uORB/topics/uORBTopics.hpp>
#include "uORBDeviceNode.hpp"

namespace nextpilot::uORB {

class DeviceNode;

template <typename U>
class DefaultQueueSize {
private:
    template <typename T>
    static constexpr uint8_t get_queue_size(decltype(T::ORB_QUEUE_LENGTH) *) {
        return T::ORB_QUEUE_LENGTH;
    }

    template <typename T>
    static constexpr uint8_t get_queue_size(...) {
        return 1;
    }

public:
    static constexpr unsigned value = get_queue_size<U>(nullptr);
};

class PublicationBase {
public:
    bool advertised() const {
        return _handle != nullptr;
    }

    bool unadvertise() {
        return (orb_unadvertise(_handle) == 0);
    }

    orb_id_t get_topic() const {
        return get_orb_meta(_orb_id);
    }

protected:
    PublicationBase(ORB_ID id) :
        _orb_id(id) {
    }

    ~PublicationBase() {
        if (_handle != nullptr) {
            // don't automatically unadvertise queued publications (eg vehicle_command)
            if (orb_get_queue_size(_handle) == 1) {
                unadvertise();
            }
        }
    }

    DeviceNode  *_handle{nullptr};
    const ORB_ID _orb_id;
};

/**
 * uORB publication wrapper class
 */
template <typename T, uint8_t ORB_QSIZE = DefaultQueueSize<T>::value>
class Publication : public PublicationBase {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     */
    Publication(ORB_ID id) :
        PublicationBase(id) {
    }
    Publication(const orb_metadata *meta) :
        PublicationBase(static_cast<ORB_ID>(meta->o_id)) {
    }

    bool advertise() {
        if (!advertised()) {
            // orb_advertise_queue(get_topic(), nullptr, ORB_QSIZE);
            _handle = DeviceNode::advertise(get_topic(), nullptr, nullptr, ORB_QSIZE);
        }
        return advertised();
    }

    /**
     * Publish the struct
     * @param data The uORB message struct we are updating.
     */
    bool publish(const T &data) {
        if (!advertised()) {
            advertise();
        }

        return (orb_publish(get_topic(), _handle, &data) == 0);
    }
};

/**
 * The publication class with data embedded.
 */
template <typename T>
class PublicationData : public Publication<T> {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     */
    PublicationData(ORB_ID id) :
        Publication<T>(id) {
    }
    PublicationData(const orb_metadata *meta) :
        Publication<T>(meta) {
    }

    T &get() {
        return _data;
    }
    void set(const T &data) {
        _data = data;
    }

    // Publishes the embedded struct.
    bool update() {
        return Publication<T>::publish(_data);
    }
    bool update(const T &data) {
        _data = data;
        return Publication<T>::publish(_data);
    }

private:
    T _data{};
};

/**
 * Base publication multi wrapper class
 */
template <typename T, uint8_t QSIZE = DefaultQueueSize<T>::value>
class PublicationMulti : public PublicationBase {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     */
    PublicationMulti(ORB_ID id) :
        PublicationBase(id) {
    }

    PublicationMulti(const orb_metadata *meta) :
        PublicationBase(static_cast<ORB_ID>(meta->o_id)) {
    }

    bool advertise() {
        if (!advertised()) {
            int instance = 0;
            // orb_advertise_multi_queue(get_topic(), nullptr, &instance, QSIZE);
            _handle = DeviceNode::advertise(get_topic(), nullptr, &instance, QSIZE);
        }

        return advertised();
    }

    /**
     * Publish the struct
     * @param data The uORB message struct we are updating.
     */
    bool publish(const T &data) {
        if (!advertised()) {
            advertise();
        }

        return (orb_publish(get_topic(), _handle, &data) == 0);
    }

    int get_instance() {
        // advertise if not already advertised
        if (advertise()) {
            return orb_get_instance(_handle);
        }

        return -1;
    }
};

/**
 * The publication multi class with data embedded.
 */
template <typename T>
class PublicationMultiData : public PublicationMulti<T> {
public:
    /**
     * Constructor
     *
     * @param meta The uORB metadata (usually from the ORB_ID() macro) for the topic.
     */
    PublicationMultiData(ORB_ID id) :
        PublicationMulti<T>(id) {
    }
    PublicationMultiData(const orb_metadata *meta) :
        PublicationMulti<T>(meta) {
    }

    T &get() {
        return _data;
    }
    void set(const T &data) {
        _data = data;
    }

    // Publishes the embedded struct.
    bool update() {
        return PublicationMulti<T>::publish(_data);
    }
    bool update(const T &data) {
        _data = data;
        return PublicationMulti<T>::publish(_data);
    }

private:
    T _data{};
};

} // namespace nextpilot::uORB

#endif //__cplusplus

#endif // __UORB_PUBLICATION_H__
