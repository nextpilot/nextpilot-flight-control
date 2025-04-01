/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/


#pragma once


#include <stdint.h>

enum class LockoutComponent : uint8_t {
    Commander          = 0,
    SystemShutdownLock = 1
};

/**
 * A board may provide a mechanism to signal that PX4 is in a critical state
 * and should not be reset/powered off.
 * The main use is when the system is armed, but also during parameter saving.
 *
 * This can be called from different threads.
 *
 * @param component calling component
 * @param enabled true if compoment is in critical state
 */
void px4_indicate_external_reset_lockout(LockoutComponent component, bool enabled);


