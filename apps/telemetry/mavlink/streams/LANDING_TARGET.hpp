/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef LANDING_TARGET_HPP
#define LANDING_TARGET_HPP

#include <uORB/topics/landing_target_pose.h>

class MavlinkStreamLandingTarget : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamLandingTarget(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "LANDING_TARGET";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_LANDING_TARGET;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _landing_target_sub.advertised() ? MAVLINK_MSG_ID_LANDING_TARGET_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamLandingTarget(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _landing_target_sub{ORB_ID(landing_target_pose)};

    bool send() override {
        landing_target_pose_s target;

        if (_landing_target_sub.update(&target)) {
            mavlink_landing_target_t msg{};

            msg.time_usec      = target.timestamp;
            msg.frame          = MAV_FRAME_LOCAL_NED;
            msg.x              = target.x_rel;
            msg.y              = target.y_rel;
            msg.z              = target.z_rel;
            msg.position_valid = target.rel_pos_valid;

            mavlink_msg_landing_target_send_struct(_mavlink->get_channel(), &msg);
            return true;
        }

        return false;
    }
};

#endif // LANDING_TARGET_HPP
