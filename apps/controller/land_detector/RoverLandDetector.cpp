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
 * @file RoverLandDetector.cpp
 * Land detection algorithm for Rovers
 *
 * @author Roman Bapst <bapstroma@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 */

#include "RoverLandDetector.h"

namespace land_detector {

bool RoverLandDetector::_get_ground_contact_state() {
    return true;
}

bool RoverLandDetector::_get_landed_state() {
    if (_vehicle_status.nav_state == vehicle_status_s::NAVIGATION_STATE_AUTO_LAND) {
        return true; // If Landing has been requested then say we have landed.

    } else {
        return !_armed; // If we are armed we are not landed.
    }
}

} // namespace land_detector
