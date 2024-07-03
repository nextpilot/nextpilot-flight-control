/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef OBSTACLE_DISTANCE_HPP
#define OBSTACLE_DISTANCE_HPP

#include <uORB/topics/obstacle_distance.h>

class MavlinkStreamObstacleDistance : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamObstacleDistance(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "OBSTACLE_DISTANCE";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_OBSTACLE_DISTANCE;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _obstacle_distance_fused_sub.advertised() ? (MAVLINK_MSG_ID_OBSTACLE_DISTANCE_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamObstacleDistance(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _obstacle_distance_fused_sub{ORB_ID(obstacle_distance_fused)};

    bool send() override {
        obstacle_distance_s obstacle_distance;

        if (_obstacle_distance_fused_sub.update(&obstacle_distance)) {
            mavlink_obstacle_distance_t msg{};

            msg.time_usec   = obstacle_distance.timestamp;
            msg.sensor_type = obstacle_distance.sensor_type;
            rt_memcpy(msg.distances, obstacle_distance.distances, sizeof(msg.distances));
            msg.increment    = 0;
            msg.min_distance = obstacle_distance.min_distance;
            msg.max_distance = obstacle_distance.max_distance;
            msg.angle_offset = obstacle_distance.angle_offset;
            msg.increment_f  = obstacle_distance.increment;
            msg.frame        = obstacle_distance.frame;

            mavlink_msg_obstacle_distance_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // OBSTACLE_DISTANCE_HPP
