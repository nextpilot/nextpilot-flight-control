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
 * @file navigator_mode.cpp
 *
 * Base class for different modes in navigator
 *
 * @author Julian Oes <julian@oes.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#include "navigator_mode.h"
#include "navigator.h"

NavigatorMode::NavigatorMode(Navigator *navigator) :
    _navigator(navigator) {
    /* set initial mission items */
    on_inactivation();
    on_inactive();
}

void NavigatorMode::run(bool active) {
    if (active) {
        if (!_active) {
            _navigator->set_mission_result_updated();
            on_activation();

        } else {
            /* periodic updates when active */
            on_active();
        }

    } else {
        /* periodic updates when inactive */
        if (_active) {
            on_inactivation();

        } else {
            on_inactive();
        }
    }

    _active = active;
}

void NavigatorMode::on_inactive() {
}

void NavigatorMode::on_inactivation() {
}

void NavigatorMode::on_activation() {
    /* invalidate position setpoint by default */
    _navigator->get_position_setpoint_triplet()->current.valid = false;
}

void NavigatorMode::on_active() {
}
