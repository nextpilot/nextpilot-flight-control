/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef MOUNT_ORIENTATION_HPP
#define MOUNT_ORIENTATION_HPP

#include <uORB/topics/mount_orientation.h>
#include <uORB/topics/vehicle_local_position.h>

class MavlinkStreamMountOrientation : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamMountOrientation(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "MOUNT_ORIENTATION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_MOUNT_ORIENTATION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _mount_orientation_sub.advertised() ? MAVLINK_MSG_ID_MOUNT_ORIENTATION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamMountOrientation(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _mount_orientation_sub{ORB_ID(mount_orientation)};
    uORB::Subscription _lpos_sub{ORB_ID(vehicle_local_position)};

    bool send() override {
        mount_orientation_s mount_orientation;

        if (_mount_orientation_sub.update(&mount_orientation)) {
            mavlink_mount_orientation_t msg{};

            msg.time_boot_ms = mount_orientation.timestamp / 1000;
            msg.roll         = math::degrees(mount_orientation.attitude_euler_angle[0]);
            msg.pitch        = math::degrees(mount_orientation.attitude_euler_angle[1]);
            msg.yaw          = math::degrees(mount_orientation.attitude_euler_angle[2]);

            vehicle_local_position_s lpos{};
            _lpos_sub.copy(&lpos);
            msg.yaw_absolute = math::degrees(matrix::wrap_2pi(lpos.heading + mount_orientation.attitude_euler_angle[2]));

            mavlink_msg_mount_orientation_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // MOUNT_ORIENTATION
