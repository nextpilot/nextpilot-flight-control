/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __UORB_H__
#define __UORB_H__

#pragma once

/**
 * @file uORB.h
 * API for the uORB lightweight object broker.
 */

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Object metadata.
 */
struct orb_metadata {
    const char    *o_name;            /**< unique object name */
    const uint16_t o_size;            /**< object size */
    const uint16_t o_size_no_padding; /**< object size w/o padding at the end (for logger) */
    const char    *o_fields;          /**< semicolon separated list of fields (with type) */
    uint8_t        o_id;              /**< ORB_ID enum */
};

typedef const struct orb_metadata *orb_id_t;

/**
 * Maximum number of multi topic instances. This must be <= 10 (because it's the last char of the node path)
 */
#if defined(CONSTRAINED_MEMORY)
#   define ORB_MULTI_MAX_INSTANCES 4
#else
#   define ORB_MULTI_MAX_INSTANCES 10
#endif // CONSTRAINED_MEMORY

/**
 * Generates a pointer to the uORB metadata structure for
 * a given topic.
 *
 * The topic must have been declared previously in scope
 * with ORB_DECLARE().
 *
 * @param _name		The name of the topic.
 */
#define ORB_ID(_name) &__orb_##_name

/**
 * Declare (prototype) the uORB metadata for a topic (used by code generators).
 *
 * @param _name		The name of the topic.
 */
#if defined(__cplusplus)
#   define ORB_DECLARE(_name) extern "C" const struct orb_metadata __orb_##_name
#else
#   define ORB_DECLARE(_name) extern const struct orb_metadata __orb_##_name
#endif //__cplusplus

/**
 * Define (instantiate) the uORB metadata for a topic.
 *
 * The uORB metadata is used to help ensure that updates and
 * copies are accessing the right data.
 *
 * Note that there must be no more than one instance of this macro
 * for each topic.
 *
 * @param _name		The name of the topic.
 * @param _struct	The structure the topic provides.
 * @param _size_no_padding	Struct size w/o padding at the end
 * @param _fields	All fields in a semicolon separated list e.g: "float[3] position;bool armed"
 * @param _orb_id_enum	ORB ID enum e.g.: ORB_ID::vehicle_status
 */
#define ORB_DEFINE(_name, _struct, _size_no_padding, _fields, _orb_id_enum) \
    const struct orb_metadata __orb_##_name = {                             \
        #_name,                                                             \
        sizeof(_struct),                                                    \
        _size_no_padding,                                                   \
        _fields,                                                            \
        _orb_id_enum};                                                      \
    struct hack

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * ORB topic advertiser handle.
 *
 * Advertiser handles are global; once obtained they can be shared freely
 * and do not need to be closed or released.
 *
 * This permits publication from interrupt context and other contexts where
 * a file-descriptor-based handle would not otherwise be in scope for the
 * publisher.
 */
typedef void *orb_advert_t;
typedef void *orb_subscr_t;

/**
 * Advertise as the publisher of a topic.
 *
 * This performs the initial advertisement of a topic; it creates the topic
 * node in /obj if required and publishes the initial data.
 *
 * Any number of advertisers may publish to a topic; publications are atomic
 * but co-ordination between publishers is not provided by the ORB.
 *
 * Internally this will call orb_advertise_multi with an instance of 0.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @param data    A pointer to the initial data to be published.
 *      For topics updated by interrupt handlers, the advertisement
 *      must be performed from non-interrupt context.
 * @return    nullptr on error, otherwise returns an object pointer
 *      that can be used to publish to the topic.
 *      If the topic in question is not known (due to an
 *      ORB_DEFINE with no corresponding ORB_DECLARE)
 *      this function will return nullptr and set errno to ENOENT.
 */
orb_advert_t orb_advertise(const struct orb_metadata *meta, const void *data);

/**
 * Advertise as the publisher of a topic.
 *
 * This performs the initial advertisement of a topic; it creates the topic
 * node in /obj if required and publishes the initial data.
 *
 * Any number of advertisers may publish to a topic; publications are atomic
 * but co-ordination between publishers is not provided by the ORB.
 *
 * Internally this will call orb_advertise_multi with an instance of 0.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @param data    A pointer to the initial data to be published.
 *      For topics updated by interrupt handlers, the advertisement
 *      must be performed from non-interrupt context.
 * @param queue_size  Maximum number of buffered elements. If this is 1, no queuing is
 *      used.
 * @return    nullptr on error, otherwise returns an object pointer
 *      that can be used to publish to the topic.
 *      If the topic in question is not known (due to an
 *      ORB_DEFINE with no corresponding ORB_DECLARE)
 *      this function will return nullptr and set errno to ENOENT.
 */
extern orb_advert_t orb_advertise_queue(const struct orb_metadata *meta, const void *data, unsigned int queue_size);

/**
 * Advertise as the publisher of a topic.
 *
 * This performs the initial advertisement of a topic; it creates the topic
 * node in /obj if required and publishes the initial data.
 *
 * Any number of advertisers may publish to a topic; publications are atomic
 * but co-ordination between publishers is not provided by the ORB.
 *
 * The multi can be used to create multiple independent instances of the same topic
 * (each instance has its own buffer).
 * This is useful for multiple publishers who publish the same topic. The subscriber
 * then subscribes to all instances and chooses which source he wants to use.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @param data    A pointer to the initial data to be published.
 *      For topics updated by interrupt handlers, the advertisement
 *      must be performed from non-interrupt context.
 * @param instance  Pointer to an integer which will yield the instance ID (0-based)
 *      of the publication. This is an output parameter and will be set to the newly
 *      created instance, ie. 0 for the first advertiser, 1 for the next and so on.
 * @return    nullptr on error, otherwise returns a handle
 *      that can be used to publish to the topic.
 *      If the topic in question is not known (due to an
 *      ORB_DEFINE with no corresponding ORB_DECLARE)
 *      this function will return nullptr and set errno to ENOENT.
 */
orb_advert_t orb_advertise_multi(const struct orb_metadata *meta, const void *data, int *instance);

/**
 * Advertise as the publisher of a topic.
 *
 * This performs the initial advertisement of a topic; it creates the topic
 * node in /obj if required and publishes the initial data.
 *
 * Any number of advertisers may publish to a topic; publications are atomic
 * but co-ordination between publishers is not provided by the ORB.
 *
 * The multi can be used to create multiple independent instances of the same topic
 * (each instance has its own buffer).
 * This is useful for multiple publishers who publish the same topic. The subscriber
 * then subscribes to all instances and chooses which source he wants to use.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @param data    A pointer to the initial data to be published.
 *      For topics updated by interrupt handlers, the advertisement
 *      must be performed from non-interrupt context.
 * @param instance  Pointer to an integer which will yield the instance ID (0-based)
 *      of the publication. This is an output parameter and will be set to the newly
 *      created instance, ie. 0 for the first advertiser, 1 for the next and so on.
 * @param queue_size  Maximum number of buffered elements. If this is 1, no queuing is
 *      used.
 * @return    nullptr on error, otherwise returns a handle
 *      that can be used to publish to the topic.
 *      If the topic in question is not known (due to an
 *      ORB_DEFINE with no corresponding ORB_DECLARE)
 *      this function will return nullptr and set errno to ENOENT.
 */
orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance, unsigned int queue_size);

/**
 * Unadvertise a topic.
 *
 * @param handle  handle returned by orb_advertise or orb_advertise_multi.
 * @return 0 on success
 */
int orb_unadvertise(orb_advert_t node);

/**
 * Publish new data to a topic.
 *
 * The data is atomically published to the topic and any waiting subscribers
 * will be notified.  Subscribers that are not waiting can check the topic
 * for updates using orb_check.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @handle    The handle returned from orb_advertise.
 * @param data    A pointer to the data to be published.
 * @return    RT_EOK on success, RT_ERROR otherwise with errno set accordingly.
 */
int orb_publish(const struct orb_metadata *meta, orb_advert_t node, const void *data);

/**
 * Advertise as the publisher of a topic.
 *
 * This performs the initial advertisement of a topic; it creates the topic
 * node in /obj if required and publishes the initial data.
 *
 * @see uORB::Manager::orb_advertise_multi() for meaning of the individual parameters
 */
static inline int orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *handle, const void *data,
                                   int *instance) {
    if (!*handle) {
        *handle = orb_advertise_multi(meta, data, instance);

        if (*handle) {
            return 0;
        }

    } else {
        return orb_publish(meta, *handle, data);
    }

    return -1;
}

/**
 * Subscribe to a topic.
 *
 * The returned value is a file descriptor that can be passed to poll()
 * in order to wait for updates to a topic, as well as topic_read,
 * orb_check.
 *
 * If there were any publications of the topic prior to the subscription,
 * an orb_check right after orb_subscribe will return true.
 *
 * Subscription will succeed even if the topic has not been advertised;
 * in this case the topic will have a timestamp of zero, it will never
 * signal a poll() event, checking will always return false and it cannot
 * be copied. When the topic is subsequently advertised, poll, check,
 * stat and copy calls will react to the initial publication that is
 * performed as part of the advertisement.
 *
 * Subscription will fail if the topic is not known to the system, i.e.
 * there is nothing in the system that has declared the topic and thus it
 * can never be published.
 *
 * Internally this will call orb_subscribe_multi with instance 0.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @return    NULL on error, otherwise returns a handle
 *      that can be used to read and update the topic.
 */
orb_subscr_t orb_subscribe(const struct orb_metadata *meta);

/**
 * Subscribe to a multi-instance of a topic.
 *
 * The returned value is a file descriptor that can be passed to poll()
 * in order to wait for updates to a topic, as well as topic_read,
 * orb_check.
 *
 * If there were any publications of the topic prior to the subscription,
 * an orb_check right after orb_subscribe_multi will return true.
 *
 * Subscription will succeed even if the topic has not been advertised;
 * in this case the topic will have a timestamp of zero, it will never
 * signal a poll() event, checking will always return false and it cannot
 * be copied. When the topic is subsequently advertised, poll, check,
 * stat and copy calls will react to the initial publication that is
 * performed as part of the advertisement.
 *
 * Subscription will fail if the topic is not known to the system, i.e.
 * there is nothing in the system that has declared the topic and thus it
 * can never be published.
 *
 * If a publisher publishes multiple instances the subscriber should
 * subscribe to each instance with orb_subscribe_multi
 * (@see orb_advertise_multi()).
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @param instance  The instance of the topic. Instance 0 matches the
 *      topic of the orb_subscribe() call, higher indices
 *      are for topics created with orb_advertise_multi().
 * @return    NULL on error, otherwise returns a handle
 *      that can be used to read and update the topic.
 *      If the topic in question is not known (due to an
 *      ORB_DEFINE_OPTIONAL with no corresponding ORB_DECLARE)
 *      this function will return -1 and set errno to ENOENT.
 */
orb_subscr_t orb_subscribe_multi(const struct orb_metadata *meta, uint8_t instance);

/**
 * Unsubscribe from a topic.
 *
 * @param handle  A handle returned from orb_subscribe.
 * @return    RT_EOK on success, RT_ERROR otherwise with errno set accordingly.
 */
int orb_unsubscribe(orb_subscr_t sub);

/**
 * Fetch data from a topic.
 *
 * This is the only operation that will reset the internal marker that
 * indicates that a topic has been updated for a subscriber. Once poll
 * or check return indicating that an updaet is available, this call
 * must be used to update the subscription.
 *
 * @param meta    The uORB metadata (usually from the ORB_ID() macro)
 *      for the topic.
 * @param handle  A handle returned from orb_subscribe.
 * @param buffer  Pointer to the buffer receiving the data, or NULL
 *      if the caller wants to clear the updated flag without
 *      using the data.
 * @return    RT_EOK on success, RT_ERROR otherwise with errno set accordingly.
 */
int orb_copy(const struct orb_metadata *meta, orb_subscr_t sub, void *buffer);

/**
 * Check whether a topic has been published to since the last orb_copy.
 *
 * This check can be used to determine whether to copy the topic when
 * not using poll(), or to avoid the overhead of calling poll() when the
 * topic is likely to have updated.
 *
 * Updates are tracked on a per-handle basis; this call will continue to
 * return true until orb_copy is called using the same handle.
 *
 * @param handle  A handle returned from orb_subscribe.
 * @param updated Set to true if the topic has been updated since the
 *      last time it was copied using this handle.
 * @return    RT_EOK if the check was successful, RT_ERROR otherwise with
 *      errno set accordingly.
 */
int orb_check(orb_subscr_t sub, bool *updated);

/**
 * Check if a topic has already been created and published (advertised)
 *
 * @param meta    ORB topic metadata.
 * @param instance  ORB instance
 * @return    RT_EOK if the topic exists, RT_ERROR otherwise.
 */
int orb_exists(const struct orb_metadata *meta, int instance);

/**
 * Get the number of published instances of a topic group
 *
 * @param meta    ORB topic metadata.
 * @return    The number of published instances of this topic
 */
extern int orb_group_count(const struct orb_metadata *meta);

/**
 * Set the minimum interval between which updates are seen for a subscription.
 *
 * If this interval is set, the subscriber will not see more than one update
 * within the period.
 *
 * Specifically, the first time an update is reported to the subscriber a timer
 * is started. The update will continue to be reported via poll and orb_check, but
 * once fetched via orb_copy another update will not be reported until the timer
 * expires.
 *
 * This feature can be used to pace a subscriber that is watching a topic that
 * would otherwise update too quickly.
 *
 * @param handle  A handle returned from orb_subscribe.
 * @param interval  An interval period in milliseconds.
 * @return    RT_EOK on success, RT_ERROR otherwise with ERRNO set accordingly.
 */
int orb_set_interval(orb_subscr_t sub, unsigned interval);

/**
 * Get the minimum interval between which updates are seen for a subscription.
 *
 * @see orb_set_interval()
 *
 * @param handle  A handle returned from orb_subscribe.
 * @param interval  The returned interval period in milliseconds.
 * @return    RT_EOK on success, RT_ERROR otherwise with ERRNO set accordingly.
 */
int orb_get_interval(orb_subscr_t sub, unsigned *interval);

/**
 * Returns the C type string from a short type in o_fields metadata, or nullptr
 * if not a short type
 */
const char *orb_get_c_type(unsigned char short_type);

/**
 * Print a topic to console. Do not call this directly, use print_message() instead.
 * @param meta orb topic metadata
 * @param data expected to be aligned to the largest member
 */
void orb_print_message_internal(const struct orb_metadata *meta, const void *data, bool print_topic_name);

#ifdef __cplusplus
}
#endif                            //__cplusplus

/* Diverse uORB header defines */ // XXX: move to better location
typedef uint8_t arming_state_t;
typedef uint8_t main_state_t;
typedef uint8_t hil_state_t;
typedef uint8_t navigation_state_t;
typedef uint8_t switch_pos_t;

#endif // __UORB_H__
