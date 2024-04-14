/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef MANUAL_CONTROL_HPP
#define MANUAL_CONTROL_HPP

#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/manual_control_switches.h>

class MavlinkStreamManualControl : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamManualControl(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "MANUAL_CONTROL";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_MANUAL_CONTROL;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _manual_control_setpoint_sub.advertised() ? (MAVLINK_MSG_ID_MANUAL_CONTROL_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) :
                                                           0;
    }

private:
    explicit MavlinkStreamManualControl(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};
    uORB::Subscription _manual_control_switches_sub{ORB_ID(manual_control_switches)};

    bool send() override {
        manual_control_setpoint_s manual_control_setpoint;

        if (_manual_control_setpoint_sub.update(&manual_control_setpoint)) {
            mavlink_manual_control_t msg{};

            msg.target = mavlink_system.sysid;
            msg.x      = manual_control_setpoint.pitch * 1000;
            msg.y      = manual_control_setpoint.roll * 1000;
            msg.z      = manual_control_setpoint.throttle * 1000;
            msg.r      = manual_control_setpoint.yaw * 1000;

            manual_control_switches_s manual_control_switches{};

            if (_manual_control_switches_sub.copy(&manual_control_switches)) {
                unsigned shift = 2;
                msg.buttons    = 0;
                msg.buttons |= (manual_control_switches.return_switch << (shift * 1));
                msg.buttons |= (manual_control_switches.loiter_switch << (shift * 3));
                msg.buttons |= (manual_control_switches.offboard_switch << (shift * 5));
                msg.buttons |= (manual_control_switches.kill_switch << (shift * 6));
            }

            mavlink_msg_manual_control_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // MANUAL_CONTROL
