/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ACTUATOR_OUTPUT_STATUS_HPP
#define ACTUATOR_OUTPUT_STATUS_HPP

#include <uORB/topics/actuator_outputs.h>

class MavlinkStreamActuatorOutputStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamActuatorOutputStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ACTUATOR_OUTPUT_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ACTUATOR_OUTPUT_STATUS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _act_output_sub.advertised() ? (MAVLINK_MSG_ID_ACTUATOR_OUTPUT_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamActuatorOutputStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _act_output_sub{ORB_ID(actuator_outputs)};

    bool send() override {
        actuator_outputs_s act;

        if (_act_output_sub.update(&act)) {
            mavlink_actuator_output_status_t msg{};

            msg.time_usec = act.timestamp;
            msg.active    = act.noutputs;

            static size_t           actuator_outputs_size               = act.noutputs;
            static constexpr size_t mavlink_actuator_output_status_size = sizeof(msg.actuator) / sizeof(msg.actuator[0]);

            for (unsigned i = 0; i < math::min(actuator_outputs_size, mavlink_actuator_output_status_size); i++) {
                msg.actuator[i] = act.output[i];
            }

            mavlink_msg_actuator_output_status_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // ACTUATOR_OUTPUT_STATUS_HPP
