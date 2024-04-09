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
 * @file VtolLandDetector.cpp
 * Land detection algorithm for VTOL
 *
 * @author Roman Bapst <bapstroma@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 */

#include <drivers/drv_hrt.h>
#include <matrix/math.hpp>

#include "VtolLandDetector.h"

namespace land_detector {

void VtolLandDetector::_update_topics() {
    MulticopterLandDetector::_update_topics();
}

bool VtolLandDetector::_get_maybe_landed_state() {
    // If in Fixed-wing mode, only trigger if disarmed
    if (_vehicle_status.vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
        return !_armed;
    }

    return MulticopterLandDetector::_get_maybe_landed_state();
}

bool VtolLandDetector::_get_landed_state() {
    // If in Fixed-wing mode, only trigger if disarmed
    if (_vehicle_status.vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
        return !_armed;
    }

    return MulticopterLandDetector::_get_landed_state();
}

bool VtolLandDetector::_get_freefall_state() {
    // true if falling or in a parabolic flight (low gravity)
    bool free_fall_detected = MulticopterLandDetector::_get_freefall_state();

    // only return a positive free fall detected if not in fixed-wing mode
    return _vehicle_status.vehicle_type != vehicle_status_s::VEHICLE_TYPE_FIXED_WING && free_fall_detected;
}

} // namespace land_detector
