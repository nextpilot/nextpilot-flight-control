/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#pragma once

#include <cstdint>
#include <atomic>

#include <px4_platform_common/sem.h>

/**
 * @class LockstepComponents
 * Allows to register components (threads) that need to be updated or waited for in every lockstep cycle (barrier).
 * Registered components need to ensure they poll on topics that is updated in every lockstep cycle.
 */
class LockstepComponents {
public:
    LockstepComponents(bool no_cleanup_on_destroy = false);
    ~LockstepComponents();

    /**
	 * Register a component
	 * @return a valid component ID > 0 or 0 on error (or unsupported)
	 */
    int  register_component();
    void unregister_component(int component);

    /**
	 * signal an update from a component
	 * @param component component ID
	 */
    void lockstep_progress(int component);

    /**
	 * Wait for all registered components to call lockstep_progress()
	 * Note: only 1 thread can call this
	 */
    void wait_for_components();

private:
    const bool _no_cleanup_on_destroy;

    px4_sem_t _components_sem;

    std::atomic_int _components_used_bitset{0};
    std::atomic_int _components_progress_bitset{0};
};
