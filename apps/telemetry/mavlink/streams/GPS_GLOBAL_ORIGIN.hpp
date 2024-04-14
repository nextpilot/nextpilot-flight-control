/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GPS_GLOBAL_ORIGIN_HPP
#define GPS_GLOBAL_ORIGIN_HPP

#include <uORB/topics/vehicle_local_position.h>

class MavlinkStreamGpsGlobalOrigin : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGpsGlobalOrigin(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GPS_GLOBAL_ORIGIN";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_vehicle_local_position_sub.advertised()) {
            return MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

    bool request_message(float param2, float param3, float param4, float param5, float param6, float param7) override {
        _force_next_send = true;
        return send();
    }

private:
    explicit MavlinkStreamGpsGlobalOrigin(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_local_position_sub{ORB_ID(vehicle_local_position)};

    uint64_t _ref_timestamp{0};
    double   _ref_lat{static_cast<double>(NAN)};
    double   _ref_lon{static_cast<double>(NAN)};
    float    _ref_alt{NAN};
    bool     _force_next_send{true};

    bool send() override {
        vehicle_local_position_s vehicle_local_position{};

        if ((_vehicle_local_position_sub.updated() || _force_next_send) && _vehicle_local_position_sub.copy(&vehicle_local_position)) {
            if (vehicle_local_position.xy_global && vehicle_local_position.z_global) {
                static constexpr double LLA_MIN_DIFF = 0.0000001; // ~11.132 mm at the equator

                if (_force_next_send || (_ref_timestamp != vehicle_local_position.ref_timestamp) || (fabs(_ref_lat - vehicle_local_position.ref_lat) > LLA_MIN_DIFF) || (fabs(_ref_lon - vehicle_local_position.ref_lon) > LLA_MIN_DIFF) || (fabsf(_ref_alt - vehicle_local_position.ref_alt) > 0.001f)) {
                    mavlink_gps_global_origin_t msg{};
                    msg.latitude  = round(vehicle_local_position.ref_lat * 1e7);   // double degree -> int32 degreeE7
                    msg.longitude = round(vehicle_local_position.ref_lon * 1e7);   // double degree -> int32 degreeE7
                    msg.altitude  = roundf(vehicle_local_position.ref_alt * 1e3f); // float m -> int32 mm
                    msg.time_usec = vehicle_local_position.ref_timestamp;          // int64 time since system boot
                    mavlink_msg_gps_global_origin_send_struct(_mavlink->get_channel(), &msg);

                    _ref_timestamp = vehicle_local_position.ref_timestamp;
                    _ref_lat       = vehicle_local_position.ref_lat;
                    _ref_lon       = vehicle_local_position.ref_lon;
                    _ref_alt       = vehicle_local_position.ref_alt;

                    _force_next_send = false;

                    return true;
                }
            }
        }

        return false;
    }
};

#endif // GPS_GLOBAL_ORIGIN_HPP
