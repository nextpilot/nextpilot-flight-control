/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ATTITUDE_HPP
#define ATTITUDE_HPP

#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_angular_velocity.h>

class MavlinkStreamAttitude : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamAttitude(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ATTITUDE";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ATTITUDE;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _att_sub.advertised() ? MAVLINK_MSG_ID_ATTITUDE_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamAttitude(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _att_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _angular_velocity_sub{ORB_ID(vehicle_angular_velocity)};

    bool send() override {
        vehicle_attitude_s att;

        if (_att_sub.update(&att)) {
            vehicle_angular_velocity_s angular_velocity{};
            _angular_velocity_sub.copy(&angular_velocity);

            mavlink_attitude_t msg{};

            const matrix::Eulerf euler = matrix::Quatf(att.q);
            msg.time_boot_ms           = att.timestamp / 1000;
            msg.roll                   = euler.phi();
            msg.pitch                  = euler.theta();
            msg.yaw                    = euler.psi();

            msg.rollspeed  = angular_velocity.xyz[0];
            msg.pitchspeed = angular_velocity.xyz[1];
            msg.yawspeed   = angular_velocity.xyz[2];

            mavlink_msg_attitude_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // ATTITUDE_HPP
