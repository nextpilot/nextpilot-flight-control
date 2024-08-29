/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <px4_platform_common/external_reset_lockout.h>

#if defined(BOARD_INDICATE_EXTERNAL_LOCKOUT_STATE)

#   include <px4_platform_common/atomic.h>

static px4::atomic<uint8_t> lockout_states{0};

void px4_indicate_external_reset_lockout(LockoutComponent component, bool enabled) {
    const uint8_t component_mask = 1 << (uint8_t)component;
    uint8_t       current_state;

    if (enabled) {
        current_state = lockout_states.fetch_or(component_mask) | component_mask;

    } else {
        current_state = lockout_states.fetch_and(~component_mask) & ~component_mask;
    }

    BOARD_INDICATE_EXTERNAL_LOCKOUT_STATE(current_state != 0);
}

#else

void px4_indicate_external_reset_lockout(LockoutComponent component, bool enabled) {
}

#endif /* BOARD_INDICATE_EXTERNAL_LOCKOUT_STATE */
