/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ORBIT_EXECUTION_STATUS_HPP
#define ORBIT_EXECUTION_STATUS_HPP

#include <uORB/topics/orbit_status.h>

class MavlinkStreamOrbitStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamOrbitStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ORBIT_EXECUTION_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ORBIT_EXECUTION_STATUS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _orbit_status_subs.advertised() ? MAVLINK_MSG_ID_ORBIT_EXECUTION_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamOrbitStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::SubscriptionMultiArray<orbit_status_s, 2> _orbit_status_subs{ORB_ID::orbit_status};

    bool send() override {
        orbit_status_s orbit_status;
        bool           updated = false;

        for (auto &orbit_sub : _orbit_status_subs) {
            if (orbit_sub.update(&orbit_status)) {
                mavlink_orbit_execution_status_t msg_orbit_execution_status{};

                msg_orbit_execution_status.time_usec = orbit_status.timestamp;
                msg_orbit_execution_status.radius    = orbit_status.radius;
                msg_orbit_execution_status.frame     = orbit_status.frame;
                msg_orbit_execution_status.x         = orbit_status.x * 1e7;
                msg_orbit_execution_status.y         = orbit_status.y * 1e7;
                msg_orbit_execution_status.z         = orbit_status.z;

                mavlink_msg_orbit_execution_status_send_struct(_mavlink->get_channel(), &msg_orbit_execution_status);

                // only one subscription should ever be active at any time, so we can exit here
                updated = true;
                break;
            }
        }

        return updated;
    }
};

#endif // ORBIT_EXECUTION_STATUS_HPP
