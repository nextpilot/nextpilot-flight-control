/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef VFR_HUD_HPP
#define VFR_HUD_HPP

#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/airspeed_validated.h>
#include <uORB/topics/vehicle_air_data.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>

class MavlinkStreamVFRHUD : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamVFRHUD(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "VFR_HUD";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_VFR_HUD;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_lpos_sub.advertised() || _airspeed_validated_sub.advertised()) {
            return MAVLINK_MSG_ID_VFR_HUD_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamVFRHUD(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _lpos_sub{ORB_ID(vehicle_local_position)};
    uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};
    uORB::Subscription _vehicle_thrust_setpoint_0_sub{ORB_ID(vehicle_thrust_setpoint), 0};
    uORB::Subscription _vehicle_thrust_setpoint_1_sub{ORB_ID(vehicle_thrust_setpoint), 1};
    uORB::Subscription _airspeed_validated_sub{ORB_ID(airspeed_validated)};
    uORB::Subscription _air_data_sub{ORB_ID(vehicle_air_data)};

    bool send() override {
        if (_lpos_sub.updated() || _airspeed_validated_sub.updated()) {
            vehicle_local_position_s lpos{};
            _lpos_sub.copy(&lpos);

            actuator_armed_s armed{};
            _armed_sub.copy(&armed);

            airspeed_validated_s airspeed_validated{};
            _airspeed_validated_sub.copy(&airspeed_validated);

            mavlink_vfr_hud_t msg{};
            msg.airspeed    = airspeed_validated.calibrated_airspeed_m_s;
            msg.groundspeed = sqrtf(lpos.vx * lpos.vx + lpos.vy * lpos.vy);
            msg.heading     = math::degrees(matrix::wrap_2pi(lpos.heading));

            if (armed.armed) {
                vehicle_thrust_setpoint_s vehicle_thrust_setpoint_0{};
                vehicle_thrust_setpoint_s vehicle_thrust_setpoint_1{};
                _vehicle_thrust_setpoint_0_sub.copy(&vehicle_thrust_setpoint_0);
                _vehicle_thrust_setpoint_1_sub.copy(&vehicle_thrust_setpoint_1);

                const float thrust_magnitude_0 = sqrtf(vehicle_thrust_setpoint_0.xyz[0] * vehicle_thrust_setpoint_0.xyz[0] +
                                                       vehicle_thrust_setpoint_0.xyz[1] * vehicle_thrust_setpoint_0.xyz[1] +
                                                       vehicle_thrust_setpoint_0.xyz[2] * vehicle_thrust_setpoint_0.xyz[2]);

                const float thrust_magnitude_1 = sqrtf(vehicle_thrust_setpoint_1.xyz[0] * vehicle_thrust_setpoint_1.xyz[0] +
                                                       vehicle_thrust_setpoint_1.xyz[1] * vehicle_thrust_setpoint_1.xyz[1] +
                                                       vehicle_thrust_setpoint_1.xyz[2] * vehicle_thrust_setpoint_1.xyz[2]);

                // VFR_HUD throttle should only be used for operator feedback.
                // VTOLs switch between vehicle_thrust_setpoint_0 and vehicle_thrust_setpoint_1. During transition there isn't a
                // a single throttle value, but this should still be a useful heuristic for operator awareness.
                msg.throttle = 100.f * math::max(thrust_magnitude_0, thrust_magnitude_1);

            } else {
                msg.throttle = 0.0f;
            }

            if (lpos.z_valid && lpos.z_global) {
                /* use local position estimate */
                msg.alt = -lpos.z + lpos.ref_alt;

            } else {
                vehicle_air_data_s air_data{};
                _air_data_sub.copy(&air_data);

                /* fall back to baro altitude */
                if (air_data.timestamp > 0) {
                    msg.alt = air_data.baro_alt_meter;
                }
            }

            if (lpos.v_z_valid) {
                msg.climb = -lpos.vz;
            }

            mavlink_msg_vfr_hud_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // VFR_HUD_HPP
