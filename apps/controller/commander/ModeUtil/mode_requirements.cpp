/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "mode_requirements.hpp"
#include <uORB/topics/vehicle_status.h>

namespace mode_util {

static inline void setRequirement(uint8_t nav_state, uint32_t &mode_requirement) {
    mode_requirement |= 1u << nav_state;
}

void getModeRequirements(uint8_t vehicle_type, failsafe_flags_s &flags) {
    flags.mode_req_angular_velocity                = 0;
    flags.mode_req_attitude                        = 0;
    flags.mode_req_local_position                  = 0;
    flags.mode_req_local_position_relaxed          = 0;
    flags.mode_req_global_position                 = 0;
    flags.mode_req_local_alt                       = 0;
    flags.mode_req_mission                         = 0;
    flags.mode_req_offboard_signal                 = 0;
    flags.mode_req_home_position                   = 0;
    flags.mode_req_wind_and_flight_time_compliance = 0;
    flags.mode_req_prevent_arming                  = 0;
    flags.mode_req_manual_control                  = 0;
    flags.mode_req_other                           = 0;

    // NAVIGATION_STATE_MANUAL
    setRequirement(vehicle_status_s::NAVIGATION_STATE_MANUAL, flags.mode_req_manual_control);

    // NAVIGATION_STATE_ALTCTL
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ALTCTL, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ALTCTL, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ALTCTL, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ALTCTL, flags.mode_req_manual_control);

    // NAVIGATION_STATE_POSCTL
    setRequirement(vehicle_status_s::NAVIGATION_STATE_POSCTL, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_POSCTL, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_POSCTL, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_POSCTL, flags.mode_req_local_position_relaxed);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_POSCTL, flags.mode_req_manual_control);

    if (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
        setRequirement(vehicle_status_s::NAVIGATION_STATE_POSCTL, flags.mode_req_global_position);
    }

    // NAVIGATION_STATE_AUTO_MISSION
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_global_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_mission);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION, flags.mode_req_wind_and_flight_time_compliance);

    // NAVIGATION_STATE_AUTO_LOITER
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER, flags.mode_req_global_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER, flags.mode_req_wind_and_flight_time_compliance);

    // NAVIGATION_STATE_AUTO_RTL
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_global_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_home_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_RTL, flags.mode_req_prevent_arming);

    // NAVIGATION_STATE_ACRO
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ACRO, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ACRO, flags.mode_req_manual_control);

    // NAVIGATION_STATE_DESCEND
    setRequirement(vehicle_status_s::NAVIGATION_STATE_DESCEND, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_DESCEND, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_DESCEND, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_DESCEND, flags.mode_req_prevent_arming);

    // NAVIGATION_STATE_TERMINATION
    setRequirement(vehicle_status_s::NAVIGATION_STATE_TERMINATION, flags.mode_req_prevent_arming);

    // NAVIGATION_STATE_OFFBOARD
    setRequirement(vehicle_status_s::NAVIGATION_STATE_OFFBOARD, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_OFFBOARD, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_OFFBOARD, flags.mode_req_offboard_signal);

    // NAVIGATION_STATE_STAB
    setRequirement(vehicle_status_s::NAVIGATION_STATE_STAB, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_STAB, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_STAB, flags.mode_req_manual_control);

    // NAVIGATION_STATE_AUTO_TAKEOFF
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF, flags.mode_req_local_alt);

    // NAVIGATION_STATE_AUTO_LAND
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LAND, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LAND, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LAND, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LAND, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_LAND, flags.mode_req_prevent_arming);

    // NAVIGATION_STATE_AUTO_FOLLOW_TARGET
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET, flags.mode_req_prevent_arming);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET, flags.mode_req_wind_and_flight_time_compliance);

    // NAVIGATION_STATE_AUTO_PRECLAND
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND, flags.mode_req_prevent_arming);

    // NAVIGATION_STATE_ORBIT
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ORBIT, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ORBIT, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ORBIT, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ORBIT, flags.mode_req_local_alt);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ORBIT, flags.mode_req_prevent_arming);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_ORBIT, flags.mode_req_wind_and_flight_time_compliance);

    // NAVIGATION_STATE_AUTO_VTOL_TAKEOFF
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF, flags.mode_req_angular_velocity);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF, flags.mode_req_attitude);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF, flags.mode_req_local_position);
    setRequirement(vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF, flags.mode_req_local_alt);

    static_assert(vehicle_status_s::NAVIGATION_STATE_MAX == 23, "update mode requirements");
}

} // namespace mode_util
