/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef PING_HPP
#define PING_HPP

class MavlinkStreamPing : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamPing(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "PING";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_PING;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_PING_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

    bool const_rate() override {
        return true;
    }

private:
    explicit MavlinkStreamPing(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uint32_t _sequence{0};

    bool send() override {
        mavlink_ping_t msg{};

        msg.time_usec        = hrt_absolute_time();
        msg.seq              = _sequence++;
        msg.target_system    = 0; // All systems
        msg.target_component = 0; // All components

        mavlink_msg_ping_send_struct(_mavlink->get_channel(), &msg);

        return true;
    }
};

#endif // PING_HPP
