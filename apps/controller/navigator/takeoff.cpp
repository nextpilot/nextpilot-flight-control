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
 * @file takeoff.cpp
 *
 * Helper class to Takeoff
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#include "takeoff.h"
#include "navigator.h"
// #include <events.h>

Takeoff::Takeoff(Navigator *navigator) :
    MissionBlock(navigator) {
}

void Takeoff::on_activation() {
    set_takeoff_position();

    // reset cruising speed to default
    _navigator->reset_cruising_speed();
}

void Takeoff::on_active() {
    struct position_setpoint_triplet_s *rep = _navigator->get_takeoff_triplet();

    if (rep->current.valid) {
        // reset the position
        set_takeoff_position();

    } else if (is_mission_item_reached_or_completed() && !_navigator->get_mission_result()->finished) {
        _navigator->get_mission_result()->finished = true;
        _navigator->set_mission_result_updated();
        _navigator->mode_completed(vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF);

        position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();

        // set loiter item so position controllers stop doing takeoff logic
        if (_navigator->get_land_detected()->landed) {
            _mission_item.nav_cmd = NAV_CMD_IDLE;

        } else {
            if (pos_sp_triplet->current.valid) {
                setLoiterItemFromCurrentPositionSetpoint(&_mission_item);

            } else {
                setLoiterItemFromCurrentPosition(&_mission_item);
            }
        }

        mission_apply_limitation(_mission_item);

        mission_item_to_position_setpoint(_mission_item, &pos_sp_triplet->current);

        _navigator->set_position_setpoint_triplet_updated();
    }
}

void Takeoff::set_takeoff_position() {
    struct position_setpoint_triplet_s *rep = _navigator->get_takeoff_triplet();

    float abs_altitude = 0.0f;

    float min_abs_altitude;

    // TODO: review this, comments are talking about home pos, the validity is checked but the
    // current altitude is used instead. Also, the "else" case does not consider the current altitude at all.
    if (_navigator->home_alt_valid()) { // only use home position if it is valid
        min_abs_altitude = _navigator->get_global_position()->alt + _navigator->get_takeoff_min_alt();

    } else { // e.g. flow
        min_abs_altitude = _navigator->get_takeoff_min_alt();
    }

    // Use altitude if it has been set. If home position is invalid use min_abs_altitude
    // events::LogLevel log_level = events::LogLevel::Disabled;

    if (rep->current.valid && PX4_ISFINITE(rep->current.alt) && _navigator->home_alt_valid()) {
        abs_altitude = rep->current.alt;

        // If the altitude suggestion is lower than home + minimum clearance, raise it and complain.
        if (abs_altitude < min_abs_altitude) {
            if (abs_altitude < min_abs_altitude - 0.1f) { // don't complain if difference is smaller than 10cm
                mavlink_log_critical(_navigator->get_mavlink_log_pub(),
                                     "Using minimum takeoff altitude: %.2f m\t", (double)_navigator->get_takeoff_min_alt());
                // log_level = events::LogLevel::Warning;
            }

            abs_altitude = min_abs_altitude;
        }

    } else {
        // Use home + minimum clearance but only notify.
        abs_altitude = min_abs_altitude;
        mavlink_log_info(_navigator->get_mavlink_log_pub(),
                         "Using minimum takeoff altitude: %.2f m\t", (double)_navigator->get_takeoff_min_alt());
        // log_level = events::LogLevel::Info;
    }

    // if (log_level != events::LogLevel::Disabled) {
    //     events::send<float>(events::ID("navigator_takeoff_min_alt"), {log_level, events::LogInternal::Info},
    //                         "Using minimum takeoff altitude: {1:.2m}",
    //                         _navigator->get_takeoff_min_alt());
    // }

    if (abs_altitude < _navigator->get_global_position()->alt) {
        // If the suggestion is lower than our current alt, let's not go down.
        abs_altitude = _navigator->get_global_position()->alt;
        mavlink_log_critical(_navigator->get_mavlink_log_pub(), "Already higher than takeoff altitude\t");
        // events::send(events::ID("navigator_takeoff_already_higher"), {events::Log::Error, events::LogInternal::Info},
        // "Already higher than takeoff altitude (not descending)");
    }

    // set current mission item to takeoff
    set_takeoff_item(&_mission_item, abs_altitude);
    _navigator->get_mission_result()->finished = false;
    _navigator->set_mission_result_updated();
    reset_mission_item_reached();

    // convert mission item to current setpoint
    struct position_setpoint_triplet_s *pos_sp_triplet = _navigator->get_position_setpoint_triplet();
    mission_apply_limitation(_mission_item);
    mission_item_to_position_setpoint(_mission_item, &pos_sp_triplet->current);

    pos_sp_triplet->previous.valid    = false;
    pos_sp_triplet->current.yaw_valid = true;
    pos_sp_triplet->next.valid        = false;

    if (rep->current.valid) {
        // Go on and check which changes had been requested
        if (PX4_ISFINITE(rep->current.yaw)) {
            pos_sp_triplet->current.yaw = rep->current.yaw;
        }

        // Set the current latitude and longitude even if they are NAN
        // NANs are handled in FlightTaskAuto.cpp
        pos_sp_triplet->current.lat = rep->current.lat;
        pos_sp_triplet->current.lon = rep->current.lon;

        // mark this as done
        memset(rep, 0, sizeof(*rep));
    }

    if (PX4_ISFINITE(pos_sp_triplet->current.lat) && PX4_ISFINITE(pos_sp_triplet->current.lon)) {
        _navigator->set_can_loiter_at_sp(true);

    } else {
        _navigator->set_can_loiter_at_sp(false);
    }

    _navigator->set_position_setpoint_triplet_updated();
}
