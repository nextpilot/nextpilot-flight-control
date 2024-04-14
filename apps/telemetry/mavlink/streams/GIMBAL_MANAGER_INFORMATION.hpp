/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GIMBAL_MANAGER_INFORMATION_HPP
#define GIMBAL_MANAGER_INFORMATION_HPP

#include <uORB/topics/gimbal_manager_information.h>

class MavlinkStreamGimbalManagerInformation : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGimbalManagerInformation(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GIMBAL_MANAGER_INFORMATION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_gimbal_manager_information_sub.advertised()) {
            return MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamGimbalManagerInformation(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _gimbal_manager_information_sub{ORB_ID(gimbal_manager_information)};

    bool send() override {
        gimbal_manager_information_s gimbal_manager_information;

        if (_gimbal_manager_information_sub.advertised() && _gimbal_manager_information_sub.copy(&gimbal_manager_information)) {
            // send out gimbal_manager_info with info from gimbal_manager_information
            mavlink_gimbal_manager_information_t msg{};

            msg.time_boot_ms     = gimbal_manager_information.timestamp / 1000;
            msg.cap_flags        = gimbal_manager_information.cap_flags;
            msg.gimbal_device_id = gimbal_manager_information.gimbal_device_id;
            msg.roll_min         = gimbal_manager_information.roll_min;
            msg.roll_max         = gimbal_manager_information.roll_max;
            msg.pitch_min        = gimbal_manager_information.pitch_min;
            msg.pitch_max        = gimbal_manager_information.pitch_max;
            msg.yaw_min          = gimbal_manager_information.yaw_min;
            msg.yaw_max          = gimbal_manager_information.yaw_max;

            mavlink_msg_gimbal_manager_information_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // GIMBAL_MANAGER_INFORMATION_HPP
