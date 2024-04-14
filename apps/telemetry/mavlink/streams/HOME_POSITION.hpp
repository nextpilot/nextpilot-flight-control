/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef HOME_POSITION_HPP
#define HOME_POSITION_HPP

#include <uORB/topics/home_position.h>

class MavlinkStreamHomePosition : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamHomePosition(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "HOME_POSITION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_HOME_POSITION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _home_sub.advertised() ? (MAVLINK_MSG_ID_HOME_POSITION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamHomePosition(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _home_sub{ORB_ID(home_position)};

    bool send() override {
        // we're sending the GPS home periodically to ensure the
        // the GCS does pick it up at one point
        home_position_s home;

        if (_home_sub.advertised() && _home_sub.copy(&home)) {
            if (home.valid_hpos) {
                mavlink_home_position_t msg{};

                msg.latitude  = home.lat * 1e7;
                msg.longitude = home.lon * 1e7;
                msg.altitude  = home.alt * 1e3f;

                msg.x = home.x;
                msg.y = home.y;
                msg.z = home.z;

                matrix::Quatf q(matrix::Eulerf(0.f, 0.f, home.yaw));
                q.copyTo(msg.q);

                msg.approach_x = 0.f;
                msg.approach_y = 0.f;
                msg.approach_z = 0.f;

                msg.time_usec = home.timestamp;

                mavlink_msg_home_position_send_struct(_mavlink->get_channel(), &msg);

                return true;
            }
        }

        return false;
    }
};

#endif // HOME_POSITION_HPP
