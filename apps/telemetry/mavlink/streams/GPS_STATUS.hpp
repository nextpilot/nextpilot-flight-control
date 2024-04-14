/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GPS_STATUS_HPP
#define GPS_STATUS_HPP

#include <uORB/topics/satellite_info.h>

class MavlinkStreamGPSStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGPSStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GPS_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GPS_STATUS;
    }

    const char *get_name() const override {
        return MavlinkStreamGPSStatus::get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _satellite_info_sub.advertised() ? (MAVLINK_MSG_ID_GPS_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamGPSStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _satellite_info_sub{ORB_ID(satellite_info)};

    bool send() override {
        satellite_info_s sat;

        if (_satellite_info_sub.update(&sat)) {
            mavlink_gps_status_t msg{};

            msg.satellites_visible = sat.count;

            size_t sat_count = math::min(static_cast<size_t>(sat.count),
                                         sizeof(msg.satellite_used) / sizeof(msg.satellite_used[0]));

            for (size_t i = 0; i < sat_count; i++) {
                msg.satellite_used[i]      = sat.used[i];
                msg.satellite_elevation[i] = sat.elevation[i];
                msg.satellite_azimuth[i]   = sat.azimuth[i];
                msg.satellite_snr[i]       = sat.snr[i];
                msg.satellite_prn[i]       = sat.prn[i];
            }

            mavlink_msg_gps_status_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // GPS_STATUS_HPP
