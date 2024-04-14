/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef TIME_ESTIMATE_TO_TARGET_HPP
#define TIME_ESTIMATE_TO_TARGET_HPP

#include <uORB/topics/rtl_time_estimate.h>

class MavlinkStreamTimeEstimateToTarget : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamTimeEstimateToTarget(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "TIME_ESTIMATE_TO_TARGET";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_TIME_ESTIMATE_TO_TARGET;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_TIME_ESTIMATE_TO_TARGET_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamTimeEstimateToTarget(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _rtl_estimate_sub{ORB_ID(rtl_time_estimate)};

    bool send() override {
        if (_rtl_estimate_sub.updated()) {
            rtl_time_estimate_s rtl_estimate{};
            _rtl_estimate_sub.copy(&rtl_estimate);

            mavlink_time_estimate_to_target_t msg{};
            msg.safe_return = static_cast<int32_t>(rtl_estimate.safe_time_estimate);

            // Set to -1 explicitly because not supported (yet)
            msg.land              = -1;
            msg.mission_next_item = -1;
            msg.mission_end       = -1;
            msg.commanded_action  = -1;

            mavlink_msg_time_estimate_to_target_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // TIME_ESTIMATE_TO_TARGET_HPP
