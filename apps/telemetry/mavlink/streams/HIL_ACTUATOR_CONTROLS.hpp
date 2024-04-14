/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef HIL_ACTUATOR_CONTROLS_HPP
#define HIL_ACTUATOR_CONTROLS_HPP

#include <uORB/topics/actuator_outputs.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_status.h>

class MavlinkStreamHILActuatorControls : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamHILActuatorControls(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "HIL_ACTUATOR_CONTROLS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _act_sub.advertised() ? MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamHILActuatorControls(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
        _act_sub = uORB::Subscription{ORB_ID(actuator_outputs_sim)};
    }

    uORB::Subscription _act_sub{ORB_ID(actuator_outputs)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};

    bool send() override {
        actuator_outputs_s act;

        if (_act_sub.update(&act)) {
            mavlink_hil_actuator_controls_t msg{};
            msg.time_usec = act.timestamp;

            for (unsigned i = 0; i < actuator_outputs_s::NUM_ACTUATOR_OUTPUTS; i++) {
                msg.controls[i] = act.output[i];
            }

            // mode (MAV_MODE_FLAG)
            msg.mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

            vehicle_control_mode_s control_mode;

            if (_vehicle_control_mode_sub.copy(&control_mode)) {
                if (control_mode.flag_control_auto_enabled) {
                    msg.mode |= MAV_MODE_FLAG_AUTO_ENABLED;
                }

                if (control_mode.flag_control_manual_enabled) {
                    msg.mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
                }

                if (control_mode.flag_control_attitude_enabled) {
                    msg.mode |= MAV_MODE_FLAG_STABILIZE_ENABLED;
                }
            }

            vehicle_status_s status;

            if (_vehicle_status_sub.copy(&status)) {
                if (status.arming_state == vehicle_status_s::ARMING_STATE_ARMED) {
                    msg.mode |= MAV_MODE_FLAG_SAFETY_ARMED;
                }

                if (status.hil_state == vehicle_status_s::HIL_STATE_ON) {
                    msg.mode |= MAV_MODE_FLAG_HIL_ENABLED;
                }

                if (status.nav_state == vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION) {
                    msg.mode |= MAV_MODE_FLAG_GUIDED_ENABLED;
                }
            }

            msg.flags = 0;

            mavlink_msg_hil_actuator_controls_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // HIL_ACTUATOR_CONTROLS_HPP
