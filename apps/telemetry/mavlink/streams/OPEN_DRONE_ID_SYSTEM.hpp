/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef OPEN_DRONE_ID_SYSTEM_HPP
#define OPEN_DRONE_ID_SYSTEM_HPP

#include <uORB/topics/home_position.h>
#include <uORB/topics/sensor_gps.h>

class MavlinkStreamOpenDroneIdSystem : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamOpenDroneIdSystem(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "OPEN_DRONE_ID_SYSTEM";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_OPEN_DRONE_ID_SYSTEM;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_vehicle_gps_position_sub.advertised() && _home_position_sub.advertised()) {
            return MAVLINK_MSG_ID_OPEN_DRONE_ID_SYSTEM_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamOpenDroneIdSystem(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _home_position_sub{ORB_ID(home_position)};
    uORB::Subscription _vehicle_gps_position_sub{ORB_ID(vehicle_gps_position)};

    bool send() override {
        sensor_gps_s    vehicle_gps_position;
        home_position_s home_position;

        if (_vehicle_gps_position_sub.update(&vehicle_gps_position) && _home_position_sub.copy(&home_position)) {
            if (vehicle_gps_position.fix_type >= 3 && home_position.valid_alt && home_position.valid_hpos) {
                mavlink_open_drone_id_system_t msg{};
                msg.target_component = 0; // 0 for broadcast
                msg.target_system    = 0; // 0 for broadcast
                // msg.id_or_mac // Only used for drone ID data received from other UAs.
                msg.operator_location_type = MAV_ODID_OPERATOR_LOCATION_TYPE_TAKEOFF;
                msg.classification_type    = MAV_ODID_CLASSIFICATION_TYPE_UNDECLARED;
                msg.operator_latitude      = home_position.lat * 1e7;
                msg.operator_longitude     = home_position.lon * 1e7;
                msg.area_count             = 1;
                msg.area_radius            = 0;
                msg.area_ceiling           = -1000;
                msg.area_floor             = -1000;
                msg.category_eu            = MAV_ODID_CATEGORY_EU_UNDECLARED;
                msg.class_eu               = MAV_ODID_CLASS_EU_UNDECLARED;
                msg.operator_altitude_geo  = home_position.alt;

                // timestamp: 32 bit Unix Timestamp in seconds since 00:00:00 01/01/2019.
                static uint64_t utc_offset_s = 1'546'300'800; // UTC seconds since 00:00:00 01/01/2019
                msg.timestamp                = vehicle_gps_position.time_utc_usec / 1e6 - utc_offset_s;

                mavlink_msg_open_drone_id_system_send_struct(_mavlink->get_channel(), &msg);

                return true;
            }
        }

        return false;
    }
};

#endif // OPEN_DRONE_ID_SYSTEM_HPP
