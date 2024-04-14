/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <uORB/topics/collision_report.h>

class MavlinkStreamCollision : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamCollision(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "COLLISION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_COLLISION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _collision_sub.advertised() ? MAVLINK_MSG_ID_COLLISION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamCollision(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _collision_sub{ORB_ID(collision_report)};

    bool send() override {
        collision_report_s report;
        bool               sent = false;

        while ((_mavlink->get_free_tx_buf() >= get_size()) && _collision_sub.update(&report)) {
            mavlink_collision_t msg = {};

            msg.src                      = report.src;
            msg.id                       = report.id;
            msg.action                   = report.action;
            msg.threat_level             = report.threat_level;
            msg.time_to_minimum_delta    = report.time_to_minimum_delta;
            msg.altitude_minimum_delta   = report.altitude_minimum_delta;
            msg.horizontal_minimum_delta = report.horizontal_minimum_delta;

            mavlink_msg_collision_send_struct(_mavlink->get_channel(), &msg);
            sent = true;
        }

        return sent;
    }
};

#endif // COLLISION_HPP
