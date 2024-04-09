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
 * @file AirshipLandDetector.cpp
 * Land detection algorithm for airships
 *
 * @author Anton Erasmus <anton@flycloudline.com>
 */

#include "AirshipLandDetector.h"

namespace land_detector {

bool AirshipLandDetector::_get_ground_contact_state() {
    return false;
}

bool AirshipLandDetector::_get_landed_state() {
    if (_vehicle_status.nav_state == vehicle_status_s::NAVIGATION_STATE_AUTO_LAND) {
        return true; // If Landing has been requested then say we have landed.

    } else {
        return !_armed; // If we are armed we are not landed.
    }
}

} // namespace land_detector
