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
#define ORB_MULTI_MAX_INSTANCES 4
#else
#define ORB_MULTI_MAX_INSTANCES 10
#endif

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
#define ORB_DECLARE(_name) extern "C" const struct orb_metadata __orb_##_name __EXPORT
#else
#define ORB_DECLARE(_name) extern const struct orb_metadata __orb_##_name __EXPORT
#endif

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

__BEGIN_DECLS

int uorb_start(void);
int uorb_status(void);
int uorb_top(char **topic_filter, int num_filters);

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
// typedef void *orb_advert_t;

__END_DECLS

/* Diverse uORB header defines */ // XXX: move to better location
typedef uint8_t arming_state_t;
typedef uint8_t main_state_t;
typedef uint8_t hil_state_t;
typedef uint8_t navigation_state_t;
typedef uint8_t switch_pos_t;

typedef uint16_t ORB_ID;

static inline const struct orb_metadata *get_orb_meta(ORB_ID orb_id) {
    return NULL;
}

#endif // __UORB_H__
