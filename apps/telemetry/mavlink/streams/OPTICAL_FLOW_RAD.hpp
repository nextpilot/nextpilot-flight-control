/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef OPTICAL_FLOW_RAD_HPP
#define OPTICAL_FLOW_RAD_HPP

#include <uORB/topics/vehicle_optical_flow.h>

class MavlinkStreamOpticalFlowRad : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamOpticalFlowRad(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "OPTICAL_FLOW_RAD";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_OPTICAL_FLOW_RAD;
    }

    const char *get_name() const override {
        return MavlinkStreamOpticalFlowRad::get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _vehicle_optical_flow_sub.advertised() ? (MAVLINK_MSG_ID_OPTICAL_FLOW_RAD_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) :
                                                        0;
    }

private:
    explicit MavlinkStreamOpticalFlowRad(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_optical_flow_sub{ORB_ID(vehicle_optical_flow)};

    bool send() override {
        vehicle_optical_flow_s flow;

        if (_vehicle_optical_flow_sub.update(&flow)) {
            mavlink_optical_flow_rad_t msg{};

            msg.time_usec           = flow.timestamp_sample;
            msg.sensor_id           = _vehicle_optical_flow_sub.get_instance();
            msg.integration_time_us = flow.integration_timespan_us;
            msg.integrated_x        = flow.pixel_flow[0];
            msg.integrated_y        = flow.pixel_flow[1];
            msg.integrated_xgyro    = flow.delta_angle[0];
            msg.integrated_ygyro    = flow.delta_angle[1];
            msg.integrated_zgyro    = flow.delta_angle[2];
            // msg.temperature = 0;
            msg.quality = flow.quality;
            // msg.time_delta_distance_us = 0;

            if (PX4_ISFINITE(flow.distance_m)) {
                msg.distance = flow.distance_m;

            } else {
                msg.distance = -1;
            }

            mavlink_msg_optical_flow_rad_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // OPTICAL_FLOW_RAD_HPP
