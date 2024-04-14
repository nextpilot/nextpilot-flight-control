/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef LOCAL_POSITION_NED_HPP
#define LOCAL_POSITION_NED_HPP

#include <uORB/topics/vehicle_local_position.h>

class MavlinkStreamLocalPositionNED : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamLocalPositionNED(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "LOCAL_POSITION_NED";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_LOCAL_POSITION_NED;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _lpos_sub.advertised() ? MAVLINK_MSG_ID_LOCAL_POSITION_NED_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamLocalPositionNED(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _lpos_sub{ORB_ID(vehicle_local_position)};

    bool send() override {
        vehicle_local_position_s lpos;

        if (_lpos_sub.update(&lpos)) {
            mavlink_local_position_ned_t msg{};

            msg.time_boot_ms = lpos.timestamp / 1000;
            msg.x            = lpos.x;
            msg.y            = lpos.y;
            msg.z            = lpos.z;
            msg.vx           = lpos.vx;
            msg.vy           = lpos.vy;
            msg.vz           = lpos.vz;

            mavlink_msg_local_position_ned_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // LOCAL_POSITION_NED_HPP
