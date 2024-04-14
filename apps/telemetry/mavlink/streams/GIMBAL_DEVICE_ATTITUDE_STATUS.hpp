/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GIMBAL_DEVICE_ATTITUDE_STATUS_HPP
#define GIMBAL_DEVICE_ATTITUDE_STATUS_HPP

#include <uORB/topics/gimbal_device_attitude_status.h>

class MavlinkStreamGimbalDeviceAttitudeStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGimbalDeviceAttitudeStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GIMBAL_DEVICE_ATTITUDE_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_gimbal_device_attitude_status_sub.advertised()) {
            return MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamGimbalDeviceAttitudeStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _gimbal_device_attitude_status_sub{ORB_ID(gimbal_device_attitude_status)};

    bool send() override {
        gimbal_device_attitude_status_s gimbal_device_attitude_status{};

        if (_gimbal_device_attitude_status_sub.update(&gimbal_device_attitude_status)) {
            if (gimbal_device_attitude_status.received_from_mavlink) {
                // If we have already received the gimbal device's attitude via
                // mavlink it is already forwarded directly and we don't need
                // to re-publish it here.
                return false;
            }

            mavlink_gimbal_device_attitude_status_t msg{};

            msg.target_system    = gimbal_device_attitude_status.target_system;
            msg.target_component = gimbal_device_attitude_status.target_component;

            msg.time_boot_ms = gimbal_device_attitude_status.timestamp / 1000;

            msg.flags = gimbal_device_attitude_status.device_flags;

            msg.q[0] = gimbal_device_attitude_status.q[0];
            msg.q[1] = gimbal_device_attitude_status.q[1];
            msg.q[2] = gimbal_device_attitude_status.q[2];
            msg.q[3] = gimbal_device_attitude_status.q[3];

            msg.angular_velocity_x = gimbal_device_attitude_status.angular_velocity_x;
            msg.angular_velocity_y = gimbal_device_attitude_status.angular_velocity_y;
            msg.angular_velocity_z = gimbal_device_attitude_status.angular_velocity_z;

            msg.failure_flags = gimbal_device_attitude_status.failure_flags;

            mavlink_msg_gimbal_device_attitude_status_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // GIMBAL_DEVICE_ATTITUDE_STATUS
