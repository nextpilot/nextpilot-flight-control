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

#include "uorb_device_node.h"

/**
 * @see uORB::Manager::orb_advertise()
 */
extern orb_advert_t orb_advertise(const struct orb_metadata *meta, const void *data) __EXPORT;

/**
 * @see uORB::Manager::orb_advertise()
 */
extern orb_advert_t orb_advertise_queue(const struct orb_metadata *meta, const void *data,
                                        unsigned int queue_size) __EXPORT;

/**
 * @see uORB::Manager::orb_advertise_multi()
 */
// extern orb_advert_t orb_advertise_multi(const struct orb_metadata *meta, const void *data, int *instance) __EXPORT;

/**
 * @see uORB::Manager::orb_advertise_multi()
 */
extern orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance,
                                              unsigned int queue_size) __EXPORT;

/**
 * @see uORB::Manager::orb_unadvertise()
 */
extern int orb_unadvertise(orb_advert_t handle) __EXPORT;

/**
 * @see uORB::Manager::orb_publish()
 */
extern int orb_publish(const struct orb_metadata *meta, orb_advert_t handle, const void *data) __EXPORT;

/**
 * Advertise as the publisher of a topic.
 *
 * This performs the initial advertisement of a topic; it creates the topic
 * node in /obj if required and publishes the initial data.
 *
 * @see uORB::Manager::orb_advertise_multi() for meaning of the individual parameters
 */
// static inline int orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *handle, const void *data,
//                                    int *instance) {
//     if (!*handle) {
//         *handle = orb_advertise_multi(meta, data, instance);

//         if (*handle) {
//             return 0;
//         }

//     } else {
//         return orb_publish(meta, *handle, data);
//     }

//     return -1;
// }

#endif // __UORB_PUBLICATION_H__
