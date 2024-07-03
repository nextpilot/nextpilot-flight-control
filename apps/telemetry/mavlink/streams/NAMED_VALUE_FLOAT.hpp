/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef NAMED_VALUE_FLOAT_HPP
#define NAMED_VALUE_FLOAT_HPP

#include <uORB/topics/debug_key_value.h>

class MavlinkStreamNamedValueFloat : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamNamedValueFloat(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "NAMED_VALUE_FLOAT";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_NAMED_VALUE_FLOAT;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _debug_key_value_sub.advertised() ? MAVLINK_MSG_ID_NAMED_VALUE_FLOAT_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamNamedValueFloat(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _debug_key_value_sub{ORB_ID(debug_key_value)};

    bool send() override {
        debug_key_value_s debug;

        if (_debug_key_value_sub.update(&debug)) {
            mavlink_named_value_float_t msg{};

            msg.time_boot_ms = debug.timestamp / 1000ULL;
            rt_memcpy(msg.name, debug.key, sizeof(msg.name));
            msg.name[sizeof(msg.name) - 1] = '\0'; // enforce null termination
            msg.value                      = debug.value;

            mavlink_msg_named_value_float_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // NAMED_VALUE_FLOAT_HPP
