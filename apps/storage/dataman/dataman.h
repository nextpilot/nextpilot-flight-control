/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file dataman.h
 *
 * DATAMANAGER driver.
 */
#pragma once

#include <string.h>
#include "navigation.h"
#include <uORB/topics/mission.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Types of items that the data manager can store */
typedef enum {
    DM_KEY_SAFE_POINTS = 0,      /* Safe points coordinates, safe point 0 is home point */
    DM_KEY_FENCE_POINTS,         /* Fence vertex coordinates */
    DM_KEY_WAYPOINTS_OFFBOARD_0, /* Mission way point coordinates sent over mavlink */
    DM_KEY_WAYPOINTS_OFFBOARD_1, /* (alternate between 0 and 1) */
    DM_KEY_MISSION_STATE,        /* Persistent mission state */
    DM_KEY_COMPAT,
    DM_KEY_NUM_KEYS /* Total number of item types defined */
} dm_item_t;

#if defined(MEMORY_CONSTRAINED_SYSTEM)
enum {
    DM_KEY_SAFE_POINTS_MAX          = 8,
    DM_KEY_FENCE_POINTS_MAX         = 16,
    DM_KEY_WAYPOINTS_OFFBOARD_0_MAX = NUM_MISSIONS_SUPPORTED,
    DM_KEY_WAYPOINTS_OFFBOARD_1_MAX = NUM_MISSIONS_SUPPORTED,
    DM_KEY_MISSION_STATE_MAX        = 1,
    DM_KEY_COMPAT_MAX               = 1
};
#else
/** The maximum number of instances for each item type */
enum {
    DM_KEY_SAFE_POINTS_MAX          = 8,
    DM_KEY_FENCE_POINTS_MAX         = 64,
    DM_KEY_WAYPOINTS_OFFBOARD_0_MAX = NUM_MISSIONS_SUPPORTED,
    DM_KEY_WAYPOINTS_OFFBOARD_1_MAX = NUM_MISSIONS_SUPPORTED,
    DM_KEY_MISSION_STATE_MAX        = 1,
    DM_KEY_COMPAT_MAX               = 1
};
#endif

struct dataman_compat_s {
    uint64_t key;
};

/* increment this define whenever a binary incompatible change is performed */
#define DM_COMPAT_VERSION 2ULL

#define DM_COMPAT_KEY ((DM_COMPAT_VERSION << 32) + (sizeof(struct mission_item_s) << 24) +                        \
                       (sizeof(struct mission_s) << 16) + (sizeof(struct mission_stats_entry_s) << 12) +          \
                       (sizeof(struct mission_fence_point_s) << 8) + (sizeof(struct mission_safe_point_s) << 4) + \
                       sizeof(struct dataman_compat_s))

/** Retrieve from the data manager store */
__EXPORT ssize_t
dm_read(
    dm_item_t item,   /* The item type to retrieve */
    unsigned  index,  /* The index of the item */
    void     *buffer, /* Pointer to caller data buffer */
    size_t    buflen  /* Length in bytes of data to retrieve */
);

/** write to the data manager store */
__EXPORT ssize_t
dm_write(
    dm_item_t   item,   /* The item type to store */
    unsigned    index,  /* The index of the item */
    const void *buffer, /* Pointer to caller data buffer */
    size_t      buflen  /* Length in bytes of data to retrieve */
);

/**
 * Lock all items of a type. Can be used for atomic updates of multiple items (single items are always updated
 * atomically).
 * Note that this lock is independent from dm_read & dm_write calls.
 * @return 0 on success and lock taken, -1 on error (lock not taken, errno set)
 */
__EXPORT int
dm_lock(
    dm_item_t item /* The item type to lock */
);

/**
 * Try to lock all items of a type (@see sem_trywait()).
 * @return 0 if lock is taken, -1 otherwise (on error or if already locked. errno is set accordingly)
 */
__EXPORT int
dm_trylock(
    dm_item_t item /* The item type to lock */
);

/** Unlock all items of a type */
__EXPORT void
dm_unlock(
    dm_item_t item /* The item type to unlock */
);

/** Erase all items of this type */
__EXPORT int
dm_clear(
    dm_item_t item /* The item type to clear */
);

#ifdef __cplusplus
}
#endif
