/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef NAV_CONTROLLER_OUTPUT_HPP
#define NAV_CONTROLLER_OUTPUT_HPP

#include <uORB/topics/position_controller_status.h>
#include <uORB/topics/tecs_status.h>
#include <uORB/topics/vehicle_global_position.h>

class MavlinkStreamNavControllerOutput : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamNavControllerOutput(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "NAV_CONTROLLER_OUTPUT";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _position_controller_status_sub.advertised() ? MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT_LEN +
                                                                  MAVLINK_NUM_NON_PAYLOAD_BYTES :
                                                              0;
    }

private:
    explicit MavlinkStreamNavControllerOutput(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _position_controller_status_sub{ORB_ID(position_controller_status)};
    uORB::Subscription _tecs_status_sub{ORB_ID(tecs_status)};
    uORB::Subscription _vehicle_global_position_sub{ORB_ID(vehicle_global_position)};

    bool send() override {
        position_controller_status_s pos_ctrl_status;

        if (_position_controller_status_sub.update(&pos_ctrl_status)) {
            tecs_status_s tecs_status{};
            _tecs_status_sub.copy(&tecs_status);

            vehicle_global_position_s vehicle_global_position{};
            _vehicle_global_position_sub.copy(&vehicle_global_position);

            mavlink_nav_controller_output_t msg{};

            msg.nav_roll       = math::degrees(pos_ctrl_status.nav_roll);
            msg.nav_pitch      = math::degrees(pos_ctrl_status.nav_pitch);
            msg.nav_bearing    = roundf(math::degrees(pos_ctrl_status.nav_bearing));
            msg.target_bearing = roundf(math::degrees(pos_ctrl_status.target_bearing));
            msg.wp_dist        = math::constrain(roundf(pos_ctrl_status.wp_dist), 0.f, (float)UINT16_MAX);
            msg.xtrack_error   = pos_ctrl_status.xtrack_error;
            msg.alt_error      = tecs_status.altitude_sp - vehicle_global_position.alt;
            msg.aspd_error     = tecs_status.true_airspeed_filtered - tecs_status.true_airspeed_sp;

            mavlink_msg_nav_controller_output_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // NAV_CONTROLLER_OUTPUT_HPP
