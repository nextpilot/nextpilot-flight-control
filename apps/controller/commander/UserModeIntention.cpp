/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "UserModeIntention.hpp"

UserModeIntention::UserModeIntention(ModuleParams *parent, const vehicle_status_s &vehicle_status,
                                     const HealthAndArmingChecks &health_and_arming_checks) :
    ModuleParams(parent), _vehicle_status(vehicle_status), _health_and_arming_checks(health_and_arming_checks) {
}

bool UserModeIntention::change(uint8_t user_intended_nav_state, bool allow_fallback, bool force) {
    _ever_had_mode_change = true;

    // Always allow mode change while disarmed
    bool always_allow = force || !isArmed();
    bool allow_change = true;

    if (!always_allow) {
        allow_change = _health_and_arming_checks.canRun(user_intended_nav_state);

        // Check fallback
        if (!allow_change && allow_fallback && _param_com_posctl_navl.get() == 0) {
            if (user_intended_nav_state == vehicle_status_s::NAVIGATION_STATE_POSCTL) {
                allow_change = _health_and_arming_checks.canRun(vehicle_status_s::NAVIGATION_STATE_ALTCTL);
                // We still use the original user intended mode. The failsafe state machine will then set the
                // fallback and once can_run becomes true, the actual user intended mode will be selected.
            }
        }
    }

    if (allow_change) {
        _had_mode_change         = true;
        _user_intented_nav_state = user_intended_nav_state;

        if (!_health_and_arming_checks.modePreventsArming(user_intended_nav_state)) {
            _nav_state_after_disarming = user_intended_nav_state;
        }
    }

    return allow_change;
}

void UserModeIntention::onDisarm() {
    _user_intented_nav_state = _nav_state_after_disarming;
}
