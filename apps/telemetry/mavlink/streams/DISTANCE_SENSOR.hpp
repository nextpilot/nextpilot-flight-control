/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef DISTANCE_SENSOR_HPP
#define DISTANCE_SENSOR_HPP

#include <uORB/Subscription.hpp>
#include <uORB/topics/distance_sensor.h>

class MavlinkStreamDistanceSensor : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamDistanceSensor(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "DISTANCE_SENSOR";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_DISTANCE_SENSOR;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _distance_sensor_subs.advertised_count() * (MAVLINK_MSG_ID_DISTANCE_SENSOR_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES);
    }

private:
    explicit MavlinkStreamDistanceSensor(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_subs{ORB_ID::distance_sensor};

    bool send() override {
        bool updated = false;

        for (int i = 0; i < _distance_sensor_subs.size(); i++) {
            distance_sensor_s dist_sensor;

            if (_distance_sensor_subs[i].update(&dist_sensor)) {
                mavlink_distance_sensor_t msg{};

                msg.time_boot_ms = dist_sensor.timestamp / 1000; /* us to ms */

                switch (dist_sensor.type) {
                case MAV_DISTANCE_SENSOR_ULTRASOUND:
                    msg.type = MAV_DISTANCE_SENSOR_ULTRASOUND;
                    break;

                case MAV_DISTANCE_SENSOR_LASER:
                    msg.type = MAV_DISTANCE_SENSOR_LASER;
                    break;

                case MAV_DISTANCE_SENSOR_INFRARED:
                    msg.type = MAV_DISTANCE_SENSOR_INFRARED;
                    break;

                default:
                    msg.type = MAV_DISTANCE_SENSOR_LASER;
                    break;
                }

                msg.current_distance = dist_sensor.current_distance * 1e2f; // m to cm
                msg.id               = i;
                msg.max_distance     = dist_sensor.max_distance * 1e2f;     // m to cm
                msg.min_distance     = dist_sensor.min_distance * 1e2f;     // m to cm
                msg.orientation      = dist_sensor.orientation;
                msg.covariance       = dist_sensor.variance * 1e4f;         // m^2 to cm^2

                mavlink_msg_distance_sensor_send_struct(_mavlink->get_channel(), &msg);

                updated = true;
            }
        }

        return updated;
    }
};

#endif // DISTANCE_SENSOR
