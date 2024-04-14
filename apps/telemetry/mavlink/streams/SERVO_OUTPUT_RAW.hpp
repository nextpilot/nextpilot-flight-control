/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef SERVO_OUTPUT_RAW_HPP
#define SERVO_OUTPUT_RAW_HPP

#include <uORB/topics/actuator_outputs.h>

template <int N>
class MavlinkStreamServoOutputRaw : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamServoOutputRaw<N>(mavlink);
    }

    static constexpr const char *get_name_static() {
        switch (N) {
        case 0:
            return "SERVO_OUTPUT_RAW_0";

        case 1:
            return "SERVO_OUTPUT_RAW_1";
        }

        return "SERVO_OUTPUT_RAW";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_SERVO_OUTPUT_RAW;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _act_sub.advertised() ? MAVLINK_MSG_ID_SERVO_OUTPUT_RAW_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamServoOutputRaw(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _act_sub{ORB_ID(actuator_outputs), N};

    bool send() override {
        actuator_outputs_s act;

        if (_act_sub.update(&act)) {
            mavlink_servo_output_raw_t msg{};

            static_assert(sizeof(act.output) / sizeof(act.output[0]) >= 16, "mavlink message requires at least 16 outputs");

            msg.time_usec   = act.timestamp;
            msg.port        = N;
            msg.servo1_raw  = act.output[0];
            msg.servo2_raw  = act.output[1];
            msg.servo3_raw  = act.output[2];
            msg.servo4_raw  = act.output[3];
            msg.servo5_raw  = act.output[4];
            msg.servo6_raw  = act.output[5];
            msg.servo7_raw  = act.output[6];
            msg.servo8_raw  = act.output[7];
            msg.servo9_raw  = act.output[8];
            msg.servo10_raw = act.output[9];
            msg.servo11_raw = act.output[10];
            msg.servo12_raw = act.output[11];
            msg.servo13_raw = act.output[12];
            msg.servo14_raw = act.output[13];
            msg.servo15_raw = act.output[14];
            msg.servo16_raw = act.output[15];

            mavlink_msg_servo_output_raw_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // SERVO_OUTPUT_RAW_HPP
