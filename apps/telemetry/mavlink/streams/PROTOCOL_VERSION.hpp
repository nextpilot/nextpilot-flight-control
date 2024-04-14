/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef PROTOCOL_VERSION_HPP
#define PROTOCOL_VERSION_HPP

class MavlinkStreamProtocolVersion : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamProtocolVersion(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "PROTOCOL_VERSION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_PROTOCOL_VERSION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_PROTOCOL_VERSION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamProtocolVersion(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        _mavlink->send_protocol_version();
        return true;
    }
};

#endif // PROTOCOL_VERSION_HPP
