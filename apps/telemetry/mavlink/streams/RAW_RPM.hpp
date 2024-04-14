/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef RAW_RPM_HPP
#define RAW_RPM_HPP

#include <uORB/topics/rpm.h>

class MavlinkStreamRawRpm : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamRawRpm(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "RAW_RPM";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_RAW_RPM;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _rpm_subs.advertised_count() * (MAVLINK_MSG_ID_RAW_RPM_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES);
    }

private:
    explicit MavlinkStreamRawRpm(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::SubscriptionMultiArray<rpm_s> _rpm_subs{ORB_ID::rpm};

    bool send() override {
        bool updated = false;

        for (int i = 0; i < _rpm_subs.size(); i++) {
            rpm_s rpm;

            if (_rpm_subs[i].update(&rpm)) {
                mavlink_raw_rpm_t msg{};

                msg.index     = i;
                msg.frequency = rpm.indicated_frequency_rpm;

                mavlink_msg_raw_rpm_send_struct(_mavlink->get_channel(), &msg);
                updated = true;
            }
        }

        return updated;
    }
};

#endif // RAW_RPM_HPP
