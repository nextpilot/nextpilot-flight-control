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

#include <uORB/topics/vehicle_status.h>
#include "HealthAndArmingChecks/HealthAndArmingChecks.hpp"
#include <px4_platform_common/module_params.hpp>

class UserModeIntention : ModuleParams {
public:
    UserModeIntention(ModuleParams *parent, const vehicle_status_s &vehicle_status,
                      const HealthAndArmingChecks &health_and_arming_checks);
    ~UserModeIntention() = default;

    /**
     * Change the user intended mode
     * @param user_intended_nav_state new mode
     * @param allow_fallback allow to fallback to a lower mode if current mode cannot run
     * @param force always set if true
     * @return true if successfully set (also if unchanged)
     */
    bool change(uint8_t user_intended_nav_state, bool allow_fallback = false, bool force = false);

    uint8_t get() const {
        return _user_intented_nav_state;
    }

    /**
     * Change the user intention to the last user intended mode where arming is possible
     */
    void onDisarm();

    /**
     * Returns false if there has not been any mode change yet
     */
    bool everHadModeChange() const {
        return _ever_had_mode_change;
    }

    bool getHadModeChangeAndClear() {
        bool ret         = _had_mode_change;
        _had_mode_change = false;
        return ret;
    }

private:
    bool isArmed() const {
        return _vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED;
    }

    const vehicle_status_s      &_vehicle_status;
    const HealthAndArmingChecks &_health_and_arming_checks;

    uint8_t _user_intented_nav_state{vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER};   ///< Current user intended mode
    uint8_t _nav_state_after_disarming{vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER}; ///< Mode that is switched into after landing/disarming

    bool _ever_had_mode_change{false}; ///< true if there was ever a mode change call (also if the same mode as already set)
    bool _had_mode_change{false};      ///< true if there was a mode change call since the last getHadModeChangeAndClear()

    DEFINE_PARAMETERS(
        (ParamInt<params_id::COM_POSCTL_NAVL>)_param_com_posctl_navl);
};
