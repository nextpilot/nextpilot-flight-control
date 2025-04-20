/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#ifndef MODULE_NAME
#   define MODULE_NAME "lockstep"
#endif

#include <lockstep_scheduler/lockstep_components.h>

#include <drivers/drv_hrt.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/tasks.h>
#include <limits.h>

LockstepComponents::LockstepComponents(bool no_cleanup_on_destroy) :
    _no_cleanup_on_destroy(no_cleanup_on_destroy) {
    px4_sem_init(&_components_sem, 0, 0);
}

LockstepComponents::~LockstepComponents() {
    // Trying to destroy a condition variable with threads currently blocked on it results in undefined behavior.
    // Therefore we allow the caller not to cleanup and let the OS take care of that.
    if (!_no_cleanup_on_destroy) {
        px4_sem_destroy(&_components_sem);
    }
}

int LockstepComponents::register_component() {
    for (int component = 0; component < (int)sizeof(int) * CHAR_BIT - 1; ++component) {
        while (true) {
            int expected = _components_used_bitset;

            if ((expected & (1 << component))) { // already used
                break;
            }

            if (_components_used_bitset.compare_exchange_weak(expected, expected | (1 << component))) {
                PX4_DEBUG("%s: got lockstep component %i", px4_get_taskname(), component);
                return 1 << component;
            }
        }
    }

    PX4_ERR("No more components left");
    return 0;
}

void LockstepComponents::unregister_component(int component) {
    if (component <= 0) {
        return;
    }

    _components_progress_bitset.fetch_and(~component);
    _components_used_bitset.fetch_and(~component);

    int components_used_bitset = _components_used_bitset;

    if (_components_progress_bitset == components_used_bitset) {
        _components_progress_bitset = 0;
        px4_sem_post(&_components_sem);
    }
}

void LockstepComponents::lockstep_progress(int component) {
    if (component <= 0) {
        return;
    }

    // Use a bitset to mark progress of each component. We could also use a simple counter,
    // but this is more robust (e.g. if a component calls this multiple times per cycle).
    int prev_value = _components_progress_bitset.fetch_or(component);

    // proceed if this is the last component setting its bit
    if ((prev_value | component) == _components_used_bitset) {
        // Note: there's a minimal race condtion here during startup: if a thread is here, and another calls
        // register_component and is fast enough it can land here as well, thus leading to 2 unlocks in a cycle.
        // That is acceptable though.
        _components_progress_bitset = 0;

        // during startup it can happen that wait_for_components() is not called yet, so avoid increasing the
        // semaphore counter more than necessary
        int value;

        if (px4_sem_getvalue(&_components_sem, &value) == 0 && value < 1) {
            px4_sem_post(&_components_sem);
        }
    }
}

void LockstepComponents::wait_for_components() {
    if (_components_used_bitset == 0) {
        return;
    }

    while (px4_sem_wait(&_components_sem) != 0) {
    }
}
