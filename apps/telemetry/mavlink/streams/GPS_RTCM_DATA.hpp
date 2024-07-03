/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GPS_RTCM_DATA_HPP
#define GPS_RTCM_DATA_HPP

#include <uORB/topics/gps_inject_data.h>

class MavlinkStreamGPSRTCMData : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGPSRTCMData(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GPS_RTCM_DATA";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GPS_RTCM_DATA;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _gps_inject_data_sub.advertised() ? (MAVLINK_MSG_ID_GPS_RTCM_DATA_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamGPSRTCMData(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _gps_inject_data_sub{ORB_ID(gps_inject_data), 0};

    bool send() override {
        gps_inject_data_s gps_inject_data;
        bool              sent = false;

        while ((_mavlink->get_free_tx_buf() >= get_size()) && _gps_inject_data_sub.update(&gps_inject_data)) {
            mavlink_gps_rtcm_data_t msg{};

            msg.len   = gps_inject_data.len;
            msg.flags = gps_inject_data.flags;
            rt_memcpy(msg.data, gps_inject_data.data, sizeof(msg.data));

            mavlink_msg_gps_rtcm_data_send_struct(_mavlink->get_channel(), &msg);

            sent = true;
        }

        return sent;
    }
};

#endif // GPS_RTCM_DATA_HPP
