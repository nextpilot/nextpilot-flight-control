/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef UAVIONIX_ADSB_OUT_DYNAMIC_HPP
#define UAVIONIX_ADSB_OUT_DYNAMIC_HPP

#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/vehicle_air_data.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/parameter_update.h>

class MavlinkStreamUavionixADSBOutDynamic : public ModuleParams, public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamUavionixADSBOutDynamic(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "UAVIONIX_ADSB_OUT_DYNAMIC";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_UAVIONIX_ADSB_OUT_DYNAMIC;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    bool const_rate() override {
        return true;
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_UAVIONIX_ADSB_OUT_DYNAMIC_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamUavionixADSBOutDynamic(Mavlink *mavlink) :
        ModuleParams(nullptr), MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_gps_position_sub{ORB_ID(vehicle_gps_position)};
    uORB::Subscription _vehicle_air_data_sub{ORB_ID(vehicle_air_data)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::ADSB_SQUAWK>)_adsb_squawk,
        (ParamInt<params_id::ADSB_EMERGC>)_adsb_emergc);

    bool send() override {
        vehicle_status_s vehicle_status;
        _vehicle_status_sub.copy(&vehicle_status);

        sensor_gps_s vehicle_gps_position;
        _vehicle_gps_position_sub.copy(&vehicle_gps_position);

        vehicle_air_data_s vehicle_air_data;
        _vehicle_air_data_sub.copy(&vehicle_air_data);

        // Required update for dynamic message is 5 [Hz]
        mavlink_uavionix_adsb_out_dynamic_t dynamic_msg = {
            .utcTime         = static_cast<uint32_t>(vehicle_gps_position.time_utc_usec / 1000000ULL),
            .gpsLat          = vehicle_gps_position.lat,
            .gpsLon          = vehicle_gps_position.lon,
            .gpsAlt          = vehicle_gps_position.alt_ellipsoid,
            .baroAltMSL      = static_cast<int32_t>(vehicle_air_data.baro_pressure_pa / 100.0f),      // convert [Pa] to [mBar]
            .accuracyHor     = static_cast<uint32_t>(vehicle_gps_position.eph * 1000.0f),             // convert [m] to [mm]
            .accuracyVert    = static_cast<uint16_t>(vehicle_gps_position.epv * 100.0f),              // convert [m] to [cm]
            .accuracyVel     = static_cast<uint16_t>(vehicle_gps_position.s_variance_m_s * 1000.f),   // convert [m/s] to [mm/s],
            .velVert         = static_cast<int16_t>(-1.0f * vehicle_gps_position.vel_d_m_s * 100.0f), // convert [m/s] to [cm/s]
            .velNS           = static_cast<int16_t>(vehicle_gps_position.vel_n_m_s * 100.0f),         // convert [m/s] to [cm/s]
            .VelEW           = static_cast<int16_t>(vehicle_gps_position.vel_e_m_s * 100.0f),         // convert [m/s] to [cm/s]
            .state           = UAVIONIX_ADSB_OUT_DYNAMIC_STATE_ON_GROUND,
            .squawk          = static_cast<uint16_t>(_adsb_squawk.get()),
            .gpsFix          = vehicle_gps_position.fix_type,
            .numSats         = vehicle_gps_position.satellites_used,
            .emergencyStatus = static_cast<uint8_t>(_adsb_emergc.get())};

        if (vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED) {
            dynamic_msg.state |= ~UAVIONIX_ADSB_OUT_DYNAMIC_STATE_ON_GROUND;
        }

        mavlink_msg_uavionix_adsb_out_dynamic_send_struct(_mavlink->get_channel(), &dynamic_msg);

        return true;
    }
};

#endif // UAVIONIX_ADSB_OUT_DYNAMIC_HPP
