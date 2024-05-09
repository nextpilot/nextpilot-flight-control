/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef SYSTEM_TIME_HPP
#define SYSTEM_TIME_HPP

class MavlinkStreamSystemTime : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamSystemTime(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "SYSTEM_TIME";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_SYSTEM_TIME;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_SYSTEM_TIME_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamSystemTime(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        timespec tv;
        clock_gettime(CLOCK_REALTIME, &tv);

        mavlink_system_time_t msg{};
        msg.time_boot_ms   = hrt_absolute_time() / 1000;
        msg.time_unix_usec = (uint64_t)tv.tv_sec * 1000000 + tv.tv_nsec / 1000;

        // If the time is before 2001-01-01, it's probably the default 2000
        // and we don't need to bother sending it because it's definitely wrong.
        if (msg.time_unix_usec > 978307200000000) {
            mavlink_msg_system_time_send_struct(_mavlink->get_channel(), &msg);
            return true;
        }

        return false;
    }
};

#endif // SYSTEM_TIME_HPP
