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

#include <events/events.h>
#include <uORB/topics/vehicle_status.h>

#include <stdint.h>

using navigation_mode_t = events::px4::enums::navigation_mode_t;

namespace mode_util {

static inline navigation_mode_t navigation_mode(uint8_t nav_state) {
    switch (nav_state) {
    case vehicle_status_s::NAVIGATION_STATE_MANUAL:
        return navigation_mode_t::manual;

    case vehicle_status_s::NAVIGATION_STATE_ALTCTL:
        return navigation_mode_t::altctl;

    case vehicle_status_s::NAVIGATION_STATE_POSCTL:
        return navigation_mode_t::posctl;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
        return navigation_mode_t::auto_mission;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER:
        return navigation_mode_t::auto_loiter;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_RTL:
        return navigation_mode_t::auto_rtl;

    case vehicle_status_s::NAVIGATION_STATE_ACRO:
        return navigation_mode_t::acro;

    case vehicle_status_s::NAVIGATION_STATE_OFFBOARD:
        return navigation_mode_t::offboard;

    case vehicle_status_s::NAVIGATION_STATE_STAB:
        return navigation_mode_t::stab;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
        return navigation_mode_t::auto_takeoff;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
        return navigation_mode_t::auto_land;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
        return navigation_mode_t::auto_follow_target;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
        return navigation_mode_t::auto_precland;

    case vehicle_status_s::NAVIGATION_STATE_ORBIT:
        return navigation_mode_t::orbit;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
        return navigation_mode_t::auto_vtol_takeoff;
    }

    static_assert(vehicle_status_s::NAVIGATION_STATE_MAX == 23, "code requires update");

    return navigation_mode_t::unknown;
}

const char *const nav_state_names[vehicle_status_s::NAVIGATION_STATE_MAX] = {
    "MANUAL",
    "ALTCTL",
    "POSCTL",
    "AUTO_MISSION",
    "AUTO_LOITER",
    "AUTO_RTL",
    "6: unallocated",
    "7: unallocated",
    "AUTO_LANDENGFAIL",
    "9: unallocated",
    "ACRO",
    "11: UNUSED",
    "DESCEND",
    "TERMINATION",
    "OFFBOARD",
    "STAB",
    "16: UNUSED2",
    "AUTO_TAKEOFF",
    "AUTO_LAND",
    "AUTO_FOLLOW_TARGET",
    "AUTO_PRECLAND",
    "ORBIT"};

} // namespace mode_util
