/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef FLIGHT_INFORMATION_HPP
#define FLIGHT_INFORMATION_HPP

#include <uORB/topics/vehicle_status.h>

class MavlinkStreamFlightInformation : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamFlightInformation(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "FLIGHT_INFORMATION";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_FLIGHT_INFORMATION;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_FLIGHT_INFORMATION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamFlightInformation(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
        _param_com_flight_uuid = param_find("COM_FLIGHT_UUID");
    }

    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
    param_t            _param_com_flight_uuid{PARAM_INVALID};

    bool send() override {
        vehicle_status_s vehicle_status{};

        if (_vehicle_status_sub.copy(&vehicle_status) && vehicle_status.timestamp != 0) {
            mavlink_flight_information_t flight_info{};
            flight_info.time_boot_ms     = hrt_absolute_time() / 1000;
            flight_info.arming_time_utc  = vehicle_status.armed_time;
            flight_info.takeoff_time_utc = vehicle_status.takeoff_time;

            int32_t flight_uuid;

            if (param_get(_param_com_flight_uuid, &flight_uuid) == PX4_OK) {
                flight_info.flight_uuid = static_cast<uint64_t>(flight_uuid);
            }

            mavlink_msg_flight_information_send_struct(_mavlink->get_channel(), &flight_info);

            return true;
        }

        return false;
    }
};

#endif // FLIGHT_INFORMATION_HPP
