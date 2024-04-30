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
 * @file custom_flight_mode.h
 * custom flight modes
 *
 */

#pragma once

#include <stdint.h>

enum CUSTOM_MAIN_FLIGHT_MODE {
    CUSTOM_MAIN_FLIGHT_MODE_MANUAL = 1,
    CUSTOM_MAIN_FLIGHT_MODE_ALTCTL,
    CUSTOM_MAIN_FLIGHT_MODE_POSCTL,
    CUSTOM_MAIN_FLIGHT_MODE_AUTO,
    CUSTOM_MAIN_FLIGHT_MODE_ACRO,
    CUSTOM_MAIN_FLIGHT_MODE_OFFBOARD,
    CUSTOM_MAIN_FLIGHT_MODE_STABILIZED,
    CUSTOM_MAIN_FLIGHT_MODE_RATTITUDE_LEGACY,
    CUSTOM_MAIN_FLIGHT_MODE_SIMPLE /* unused, but reserved for future use */
};

enum CUSTOM_SUB_FLIGHT_MODE_AUTO {
    CUSTOM_SUB_FLIGHT_MODE_AUTO_READY = 1,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_TAKEOFF,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_LOITER,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_MISSION,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_RTL,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_LAND,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_RESERVED_DO_NOT_USE, // was CUSTOM_SUB_FLIGHT_MODE_AUTO_RTGS, deleted 2020-03-05
    CUSTOM_SUB_FLIGHT_MODE_AUTO_FOLLOW_TARGET,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_PRECLAND,
    CUSTOM_SUB_FLIGHT_MODE_AUTO_VTOL_TAKEOFF
};

enum CUSTOM_SUB_FLIGHT_MODE_POSCTL {
    CUSTOM_SUB_FLIGHT_MODE_POSCTL_POSCTL = 0,
    CUSTOM_SUB_FLIGHT_MODE_POSCTL_ORBIT
};

union custom_flight_mode {
    struct {
        uint16_t reserved;
        uint8_t  main_mode;
        uint8_t  sub_mode;
    };

    uint32_t data;
    float    data_float;

    struct {
        uint16_t reserved_hl;
        uint16_t custom_mode_hl;
    };
};

#ifdef DEFINE_GET_CUSTOM_MODE
#   include <uORB/topics/vehicle_status.h>

static inline union custom_flight_mode get_custom_flight_mode(uint8_t nav_state) {
    union custom_flight_mode custom_mode;
    custom_mode.data = 0;

    switch (nav_state) {
    case vehicle_status_s::NAVIGATION_STATE_MANUAL:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_MANUAL;
        break;

    case vehicle_status_s::NAVIGATION_STATE_ALTCTL:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_ALTCTL;
        break;

    case vehicle_status_s::NAVIGATION_STATE_POSCTL:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_POSCTL;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_MISSION;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_LOITER;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_RTL:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_RTL;
        break;

    case vehicle_status_s::NAVIGATION_STATE_ACRO:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_ACRO;
        break;

    case vehicle_status_s::NAVIGATION_STATE_DESCEND:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_LAND;
        break;

    case vehicle_status_s::NAVIGATION_STATE_TERMINATION:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_MANUAL;
        break;

    case vehicle_status_s::NAVIGATION_STATE_OFFBOARD:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_OFFBOARD;
        break;

    case vehicle_status_s::NAVIGATION_STATE_STAB:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_STABILIZED;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_TAKEOFF;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_LAND:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_LAND;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_FOLLOW_TARGET:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_FOLLOW_TARGET;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_PRECLAND:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_PRECLAND;
        break;

    case vehicle_status_s::NAVIGATION_STATE_ORBIT:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_POSCTL;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_POSCTL_ORBIT;
        break;

    case vehicle_status_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF:
        custom_mode.main_mode = CUSTOM_MAIN_FLIGHT_MODE_AUTO;
        custom_mode.sub_mode  = CUSTOM_SUB_FLIGHT_MODE_AUTO_VTOL_TAKEOFF;
        break;
    }

    return custom_mode;
}

#endif /* DEFINE_GET_CUSTOM_MODE */
