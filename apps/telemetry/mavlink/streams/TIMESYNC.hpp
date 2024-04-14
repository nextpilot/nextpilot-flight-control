/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef TIMESYNC_HPP
#define TIMESYNC_HPP

#include <drivers/drv_hrt.h>

class MavlinkStreamTimesync : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamTimesync(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "TIMESYNC";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_TIMESYNC;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_TIMESYNC_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamTimesync(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        mavlink_timesync_t msg{};

        msg.tc1 = 0;
        msg.ts1 = hrt_absolute_time() * 1000; // boot time in nanoseconds

        mavlink_msg_timesync_send_struct(_mavlink->get_channel(), &msg);

        return true;
    }
};

#endif // TIMESYNC_HPP
