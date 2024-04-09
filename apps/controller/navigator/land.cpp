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
 * @file land.cpp
 *
 * Helper class to land at the current position
 *
 * @author Andreas Antener <andreas@uaventure.com>
 */

#include "land.h"
#include "navigator.h"

Land::Land(Navigator *navigator) :
    MissionBlock(navigator) {
}

void Land::on_activation() {
    /* set current mission item to Land */
    set_land_item(&_mission_item);
    _navigator->get_mission_result()->finished = false;
    _navigator->set_mission_result_updated();
    reset_mission_item_reached();

    /* convert mission item to current setpoint */
    struct position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();
    pos_sp_triplet->previous.valid                     = false;
    mission_apply_limitation(_mission_item);
    mission_item_to_position_setpoint(_mission_item, &pos_sp_triplet->current);
    pos_sp_triplet->next.valid = false;

    _navigator->set_can_loiter_at_sp(false);

    _navigator->set_position_setpoint_triplet_updated();

    // reset cruising speed to default
    _navigator->reset_cruising_speed();
}

void Land::on_active() {
    /* for VTOL update landing location during back transition */
    if (_navigator->get_vstatus()->is_vtol &&
        _navigator->get_vstatus()->in_transition_mode) {
        struct position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();

        // create a virtual wp 1m in front of the vehicle to track during the backtransition
        waypoint_from_heading_and_distance(_navigator->get_global_position()->lat, _navigator->get_global_position()->lon,
                                           _navigator->get_position_setpoint_triplet()->current.yaw, 1.f,
                                           &pos_sp_triplet->current.lat, &pos_sp_triplet->current.lon);

        _navigator->set_position_setpoint_triplet_updated();
    }

    if (_navigator->get_land_detected()->landed) {
        _navigator->get_mission_result()->finished = true;
        _navigator->set_mission_result_updated();
        _navigator->mode_completed(vehicle_status_s::NAVIGATION_STATE_AUTO_LAND);
        set_idle_item(&_mission_item);

        struct position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();
        mission_item_to_position_setpoint(_mission_item, &pos_sp_triplet->current);
        _navigator->set_position_setpoint_triplet_updated();
    }

    /* check if landing needs to be aborted */
    if (_navigator->abort_landing()) {
        // send reposition cmd to get out of land mode (will loiter at current position and altitude)
        vehicle_command_s vcmd = {};

        vcmd.command = vehicle_command_s::VEHICLE_CMD_DO_REPOSITION;
        vcmd.param1  = -1;
        vcmd.param2  = 1;
        vcmd.param5  = _navigator->get_global_position()->lat;
        vcmd.param6  = _navigator->get_global_position()->lon;
        // as we don't know the landing point altitude assume the worst case (abort at 0m above ground),
        // and thus always climb MIS_LND_ABRT_ALT
        vcmd.param7 = _navigator->get_global_position()->alt + _navigator->get_landing_abort_min_alt();

        _navigator->publish_vehicle_cmd(&vcmd);
    }
}
