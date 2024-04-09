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
 * @file dataman_mocks.h
 * Provides a minimal dataman implementation to compile against for testing
 *
 * @author Roman Bapst
 * @author Julian Kent
 */
#pragma once

#include <dataman/dataman.h>
extern "C" {
__EXPORT ssize_t
dm_read(
    dm_item_t item,   /* The item type to retrieve */
    unsigned  index,  /* The index of the item */
    void     *buffer, /* Pointer to caller data buffer */
    size_t    buflen  /* Length in bytes of data to retrieve */
) {
    return 0;
};

/** write to the data manager store */
__EXPORT ssize_t
dm_write(
    dm_item_t   item,   /* The item type to store */
    unsigned    index,  /* The index of the item */
    const void *buffer, /* Pointer to caller data buffer */
    size_t      buflen  /* Length in bytes of data to retrieve */
) {
    return 0;
};

/**
 * Lock all items of a type. Can be used for atomic updates of multiple items (single items are always updated
 * atomically).
 * Note that this lock is independent from dm_read & dm_write calls.
 * @return 0 on success and lock taken, -1 on error (lock not taken, errno set)
 */
__EXPORT int
dm_lock(
    dm_item_t item /* The item type to lock */
) {
    return 0;
};

/**
 * Try to lock all items of a type (@see sem_trywait()).
 * @return 0 if lock is taken, -1 otherwise (on error or if already locked. errno is set accordingly)
 */
__EXPORT int
dm_trylock(
    dm_item_t item /* The item type to lock */
) {
    return 0;
};

/** Unlock all items of a type */
__EXPORT void
dm_unlock(
    dm_item_t item /* The item type to unlock */
) {};

/** Erase all items of this type */
__EXPORT int
dm_clear(
    dm_item_t item /* The item type to clear */
) {
    return 0;
};
}
