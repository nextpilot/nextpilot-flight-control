/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef AUTOPILOT_VERSION_HPP
#define AUTOPILOT_VERSION_HPP

class MavlinkStreamAutopilotVersion : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamAutopilotVersion(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "AUTOPILOT_VERSION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_AUTOPILOT_VERSION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_AUTOPILOT_VERSION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamAutopilotVersion(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        return _mavlink->send_autopilot_capabilities();
    }
};

#endif // AUTOPILOT_VERSION_HPP
