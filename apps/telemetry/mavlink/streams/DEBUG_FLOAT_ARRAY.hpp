/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef DEBUG_FLOAT_ARRAY_HPP
#define DEBUG_FLOAT_ARRAY_HPP

#include <uORB/topics/debug_array.h>

class MavlinkStreamDebugFloatArray : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamDebugFloatArray(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "DEBUG_FLOAT_ARRAY";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_DEBUG_FLOAT_ARRAY;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _debug_array_sub.advertised() ? MAVLINK_MSG_ID_DEBUG_FLOAT_ARRAY_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamDebugFloatArray(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _debug_array_sub{ORB_ID(debug_array)};

    bool send() override {
        debug_array_s debug;

        if (_debug_array_sub.update(&debug)) {
            mavlink_debug_float_array_t msg{};

            msg.time_usec = debug.timestamp;
            msg.array_id  = debug.id;
            rt_memcpy(msg.name, debug.name, sizeof(msg.name));
            msg.name[sizeof(msg.name) - 1] = '\0'; // enforce null termination

            for (size_t i = 0; i < debug_array_s::ARRAY_SIZE; i++) {
                msg.data[i] = debug.data[i];
            }

            mavlink_msg_debug_float_array_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // DEBUG_FLOAT_ARRAY_HPP
