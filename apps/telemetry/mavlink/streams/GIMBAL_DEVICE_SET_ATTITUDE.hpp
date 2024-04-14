/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GIMBAL_DEVICE_SET_ATTITUDE_HPP
#define GIMBAL_DEVICE_SET_ATTITUDE_HPP

#include <uORB/topics/gimbal_device_set_attitude.h>

class MavlinkStreamGimbalDeviceSetAttitude : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGimbalDeviceSetAttitude(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GIMBAL_DEVICE_SET_ATTITUDE";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GIMBAL_DEVICE_SET_ATTITUDE;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_gimbal_device_set_attitude_sub.advertised()) {
            return (MAVLINK_MSG_ID_GIMBAL_DEVICE_SET_ATTITUDE_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES);
        }

        return 0;
    }

private:
    explicit MavlinkStreamGimbalDeviceSetAttitude(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _gimbal_device_set_attitude_sub{ORB_ID(gimbal_device_set_attitude)};

    bool send() override {
        gimbal_device_set_attitude_s gimbal_device_set_attitude;

        if (_gimbal_device_set_attitude_sub.advertised() && _gimbal_device_set_attitude_sub.copy(&gimbal_device_set_attitude)) {
            mavlink_gimbal_device_set_attitude_t msg{};

            msg.target_system    = gimbal_device_set_attitude.target_system;
            msg.target_component = gimbal_device_set_attitude.target_component;

            msg.flags = gimbal_device_set_attitude.flags;

            msg.q[0] = gimbal_device_set_attitude.q[0];
            msg.q[1] = gimbal_device_set_attitude.q[1];
            msg.q[2] = gimbal_device_set_attitude.q[2];
            msg.q[3] = gimbal_device_set_attitude.q[3];

            msg.angular_velocity_x = gimbal_device_set_attitude.angular_velocity_x;
            msg.angular_velocity_y = gimbal_device_set_attitude.angular_velocity_y;
            msg.angular_velocity_z = gimbal_device_set_attitude.angular_velocity_z;

            mavlink_msg_gimbal_device_set_attitude_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // GIMBAL_DEVICE_SET_ATTITUDE_HPP
