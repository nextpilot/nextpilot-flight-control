/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ESC_INFO_HPP
#define ESC_INFO_HPP

#include <uORB/topics/esc_status.h>

class MavlinkStreamESCInfo : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamESCInfo(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ESC_INFO";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ESC_INFO;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        static constexpr unsigned size_per_batch = MAVLINK_MSG_ID_ESC_INFO_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        return _esc_status_sub.advertised() ? size_per_batch * _number_of_batches : 0;
    }

private:
    explicit MavlinkStreamESCInfo(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _esc_status_sub{ORB_ID(esc_status)};
    uint8_t            _number_of_batches{0};

    bool send() override {
        static constexpr uint8_t batch_size = MAVLINK_MSG_ESC_INFO_FIELD_TEMPERATURE_LEN;
        esc_status_s             esc_status;

        if (_esc_status_sub.update(&esc_status)) {
            mavlink_esc_info_t msg{};

            msg.time_usec       = esc_status.timestamp;
            msg.counter         = esc_status.counter;
            msg.count           = esc_status.esc_count;
            msg.connection_type = esc_status.esc_connectiontype;
            msg.info            = esc_status.esc_online_flags;

            // Ceil value of integer division. For 1-4 esc => 1 batch, 5-8 esc => 2 batches etc
            _number_of_batches = ceilf((float)esc_status.esc_count / batch_size);

            for (int batch_number = 0; batch_number < _number_of_batches; batch_number++) {
                msg.index = batch_number * batch_size;

                for (int esc_index = 0; esc_index < batch_size; esc_index++) {
                    msg.failure_flags[esc_index] = esc_status.esc[esc_index].failures;
                    msg.error_count[esc_index]   = esc_status.esc[esc_index].esc_errorcount;
                    msg.temperature[esc_index]   = static_cast<int16_t>(esc_status.esc[esc_index].esc_temperature *
                                                                      100.f); // convert to centiDegrees
                }

                mavlink_msg_esc_info_send_struct(_mavlink->get_channel(), &msg);
            }

            return true;
        }

        return false;
    }
};

#endif // ESC_INFO_HPP
