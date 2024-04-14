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

#include <stdint.h>

namespace gimbal {

/**
 * @struct ControlData
 * This defines the common API between an input and an output of the gimbal driver.
 * Each output must support the (full) set of the commands, and an input can create all
 * or a subset of the types.
 */
struct ControlData {
    enum class Type {
        Neutral = 0,
        Angle,
        LonLat
    };

    union TypeData {
        struct TypeAngle {
            float q[4];
            float angular_velocity[3];

            enum class Frame : uint8_t {
                AngleBodyFrame     = 0, // Also called follow mode, angle relative to vehicle forward (usually default for yaw axis).
                AngularRate        = 1, // Angular rate set only.
                AngleAbsoluteFrame = 2  // Also called lock mode, angle relative to horizon/world, lock mode. (usually default for roll and pitch).
            } frames[3];
        } angle;

        struct TypeLonLat {
            double lon;               // longitude in deg
            double lat;               // latitude in deg
            float  altitude;          // altitude in m
            float  roll_offset;       // roll offset in rad
            float  pitch_offset;      // pitch offset in rad
            float  yaw_offset;        // yaw offset in rad
            float  pitch_fixed_angle; // ignored if < -pi, otherwise use a fixed pitch angle instead of the altitude
        } lonlat;
    } type_data;

    Type type = Type::Neutral;

    uint8_t sysid_primary_control  = 0; // The MAVLink system ID selected to be in control, 0 for no one.
    uint8_t compid_primary_control = 0; // The MAVLink component ID selected to be in control, 0 for no one.
    // uint8_t sysid_secondary_control = 0; // The MAVLink system ID selected for additional input, not implemented yet.
    // uint8_t compid_secondary_control = 0; // The MAVLink component ID selected for additional input, not implemented yet.
    uint8_t device_compid = 0;
};

} /* namespace gimbal */
