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
 * @file loiter.cpp
 *
 * Helper class to loiter
 *
 * @author Julian Oes <julian@oes.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#include "loiter.h"
#include "navigator.h"

Loiter::Loiter(Navigator *navigator) :
    MissionBlock(navigator),
    ModuleParams(navigator) {
}

void Loiter::on_inactive() {
    _loiter_pos_set = false;
}

void Loiter::on_activation() {
    if (_navigator->get_reposition_triplet()->current.valid && hrt_elapsed_time(&_navigator->get_reposition_triplet()->current.timestamp) < 500_ms) {
        reposition();

    } else {
        // this is executed when the flight mode is switched to Hold manually, not through a reposition
        set_loiter_position();
    }

    // reset cruising speed to default
    _navigator->reset_cruising_speed();
}

void Loiter::on_active() {
    if (_navigator->get_reposition_triplet()->current.valid && hrt_elapsed_time(&_navigator->get_reposition_triplet()->current.timestamp) < 500_ms) {
        reposition();
    }

    // reset the loiter position if we get disarmed
    if (_navigator->get_vstatus()->arming_state != vehicle_status_s::ARMING_STATE_ARMED) {
        _loiter_pos_set = false;
    }
}

void Loiter::set_loiter_position() {
    if (_navigator->get_vstatus()->arming_state != vehicle_status_s::ARMING_STATE_ARMED &&
        _navigator->get_land_detected()->landed) {
        // Not setting loiter position if disarmed and landed, instead mark the current
        // setpoint as invalid and idle (both, just to be sure).

        _navigator->set_can_loiter_at_sp(false);
        _navigator->get_position_setpoint_triplet()->current.type = position_setpoint_s::SETPOINT_TYPE_IDLE;
        _navigator->set_position_setpoint_triplet_updated();
        _loiter_pos_set = false;
        return;

    } else if (_loiter_pos_set) {
        // Already set, nothing to do.
        return;
    }

    _loiter_pos_set = true;

    position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();

    if (_navigator->get_land_detected()->landed) {
        _mission_item.nav_cmd = NAV_CMD_IDLE;

    } else {
        if (_navigator->get_vstatus()->vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
            setLoiterItemFromCurrentPositionWithBreaking(&_mission_item);

        } else {
            setLoiterItemFromCurrentPosition(&_mission_item);
        }
    }

    // convert mission item to current setpoint
    pos_sp_triplet->previous.valid = false;
    mission_apply_limitation(_mission_item);
    mission_item_to_position_setpoint(_mission_item, &pos_sp_triplet->current);
    pos_sp_triplet->next.valid = false;

    _navigator->set_can_loiter_at_sp(pos_sp_triplet->current.type == position_setpoint_s::SETPOINT_TYPE_LOITER);
    _navigator->set_position_setpoint_triplet_updated();
}

void Loiter::reposition() {
    // we can't reposition if we are not armed yet
    if (_navigator->get_vstatus()->arming_state != vehicle_status_s::ARMING_STATE_ARMED) {
        return;
    }

    struct position_setpoint_triplet_s *rep = _navigator->get_reposition_triplet();

    if (rep->current.valid) {
        // set loiter position based on reposition command

        // convert mission item to current setpoint
        struct position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();
        pos_sp_triplet->previous.yaw                       = _navigator->get_local_position()->heading;
        pos_sp_triplet->previous.lat                       = _navigator->get_global_position()->lat;
        pos_sp_triplet->previous.lon                       = _navigator->get_global_position()->lon;
        pos_sp_triplet->previous.alt                       = _navigator->get_global_position()->alt;
        memcpy(&pos_sp_triplet->current, &rep->current, sizeof(rep->current));
        pos_sp_triplet->next.valid = false;

        _navigator->set_can_loiter_at_sp(pos_sp_triplet->current.type == position_setpoint_s::SETPOINT_TYPE_LOITER);
        _navigator->set_position_setpoint_triplet_updated();

        // mark this as done
        memset(rep, 0, sizeof(*rep));
    }
}
