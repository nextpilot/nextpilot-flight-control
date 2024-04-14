/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GIMBAL_MANAGER_STATUS_HPP
#define GIMBAL_MANAGER_STATUS_HPP

#include <uORB/topics/gimbal_manager_status.h>

class MavlinkStreamGimbalManagerStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGimbalManagerStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GIMBAL_MANAGER_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_gimbal_manager_status_sub.advertised()) {
            return MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamGimbalManagerStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _gimbal_manager_status_sub{ORB_ID(gimbal_manager_status)};

    bool send() override {
        gimbal_manager_status_s gimbal_manager_status;

        if (_gimbal_manager_status_sub.advertised() && _gimbal_manager_status_sub.copy(&gimbal_manager_status)) {
            mavlink_gimbal_manager_status_t msg{};

            msg.time_boot_ms             = gimbal_manager_status.timestamp / 1000;
            msg.flags                    = gimbal_manager_status.flags;
            msg.gimbal_device_id         = gimbal_manager_status.gimbal_device_id;
            msg.primary_control_sysid    = gimbal_manager_status.primary_control_sysid;
            msg.primary_control_compid   = gimbal_manager_status.primary_control_compid;
            msg.secondary_control_sysid  = gimbal_manager_status.secondary_control_sysid;
            msg.secondary_control_compid = gimbal_manager_status.secondary_control_compid;

            mavlink_msg_gimbal_manager_status_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // GIMBAL_MANAGER_STATUS_HPP
