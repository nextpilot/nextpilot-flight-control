/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "input.h"

namespace gimbal {

InputBase::InputBase(Parameters &parameters) :
    _parameters(parameters) {
}

void InputBase::control_data_set_lon_lat(ControlData &control_data, double lon, double lat, float altitude,
                                         float roll_angle,
                                         float pitch_fixed_angle) {
    control_data.type                               = ControlData::Type::LonLat;
    control_data.type_data.lonlat.lon               = lon;
    control_data.type_data.lonlat.lat               = lat;
    control_data.type_data.lonlat.altitude          = altitude;
    control_data.type_data.lonlat.roll_offset       = roll_angle;
    control_data.type_data.lonlat.pitch_fixed_angle = pitch_fixed_angle;
    control_data.type_data.lonlat.pitch_offset      = 0.f;
    control_data.type_data.lonlat.yaw_offset        = 0.f;
}

} /* namespace gimbal */
