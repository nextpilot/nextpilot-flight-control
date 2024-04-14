/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef GLOBAL_POSITION_INT_HPP
#define GLOBAL_POSITION_INT_HPP

#include <uORB/topics/home_position.h>
#include <uORB/topics/vehicle_air_data.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_local_position.h>

class MavlinkStreamGlobalPositionInt : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamGlobalPositionInt(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "GLOBAL_POSITION_INT";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_GLOBAL_POSITION_INT;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _gpos_sub.advertised() ? MAVLINK_MSG_ID_GLOBAL_POSITION_INT_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamGlobalPositionInt(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _gpos_sub{ORB_ID(vehicle_global_position)};
    uORB::Subscription _lpos_sub{ORB_ID(vehicle_local_position)};
    uORB::Subscription _home_sub{ORB_ID(home_position)};
    uORB::Subscription _air_data_sub{ORB_ID(vehicle_air_data)};

    bool send() override {
        vehicle_global_position_s gpos;
        vehicle_local_position_s  lpos;

        if (_gpos_sub.update(&gpos) && _lpos_sub.update(&lpos)) {
            mavlink_global_position_int_t msg{};

            if (lpos.z_valid && lpos.z_global) {
                msg.alt = (-lpos.z + lpos.ref_alt) * 1000.0f;

            } else {
                // fall back to baro altitude
                vehicle_air_data_s air_data{};
                _air_data_sub.copy(&air_data);

                if (air_data.timestamp > 0) {
                    msg.alt = air_data.baro_alt_meter * 1000.0f;
                }
            }

            home_position_s home{};
            _home_sub.copy(&home);

            if ((home.timestamp > 0) && home.valid_alt) {
                if (lpos.z_valid) {
                    msg.relative_alt = -(lpos.z - home.z) * 1000.0f;

                } else {
                    msg.relative_alt = msg.alt - (home.alt * 1000.0f);
                }

            } else {
                if (lpos.z_valid) {
                    msg.relative_alt = -lpos.z * 1000.0f;
                }
            }

            msg.time_boot_ms = gpos.timestamp / 1000;
            msg.lat          = gpos.lat * 1e7;
            msg.lon          = gpos.lon * 1e7;

            msg.vx = lpos.vx * 100.0f;
            msg.vy = lpos.vy * 100.0f;
            msg.vz = lpos.vz * 100.0f;

            msg.hdg = math::degrees(matrix::wrap_2pi(lpos.heading)) * 100.0f;

            mavlink_msg_global_position_int_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // GLOBAL_POSITION_INT_HPP
