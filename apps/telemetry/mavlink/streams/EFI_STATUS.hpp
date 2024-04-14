/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef EFI_STATUS_HPP
#define EFI_STATUS_HPP

#include <uORB/topics/internal_combustion_engine_status.h>

class MavlinkStreamEfiStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamEfiStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "EFI_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_EFI_STATUS;
    }

    const char *get_name() const override {
        return MavlinkStreamEfiStatus::get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _internal_combustion_engine_status_sub.advertised() ? MAVLINK_MSG_ID_EFI_STATUS + MAVLINK_NUM_NON_PAYLOAD_BYTES :
                                                                     0;
    }

private:
    explicit MavlinkStreamEfiStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _internal_combustion_engine_status_sub{ORB_ID(internal_combustion_engine_status)};

    bool send() override {
        internal_combustion_engine_status_s internal_combustion_engine_status;

        if (_internal_combustion_engine_status_sub.update(&internal_combustion_engine_status)) {
            mavlink_efi_status_t msg{};

            msg.health                      = internal_combustion_engine_status.state;
            msg.ecu_index                   = internal_combustion_engine_status.ecu_index;
            msg.rpm                         = internal_combustion_engine_status.engine_speed_rpm;
            msg.fuel_consumed               = internal_combustion_engine_status.estimated_consumed_fuel_volume_cm3;
            msg.fuel_flow                   = internal_combustion_engine_status.fuel_consumption_rate_cm3pm;
            msg.engine_load                 = internal_combustion_engine_status.engine_load_percent;
            msg.throttle_position           = internal_combustion_engine_status.throttle_position_percent;
            msg.spark_dwell_time            = internal_combustion_engine_status.spark_dwell_time_ms;
            msg.barometric_pressure         = internal_combustion_engine_status.atmospheric_pressure_kpa;
            msg.intake_manifold_pressure    = internal_combustion_engine_status.intake_manifold_pressure_kpa;
            msg.intake_manifold_temperature = internal_combustion_engine_status.intake_manifold_temperature;

            msg.cylinder_head_temperature = internal_combustion_engine_status.cylinder_head_temperature;
            msg.ignition_timing           = internal_combustion_engine_status.ignition_timing_deg;
            msg.injection_time            = internal_combustion_engine_status.injection_time_ms;
            msg.exhaust_gas_temperature   = internal_combustion_engine_status.exhaust_gas_temperature;
            msg.throttle_out              = internal_combustion_engine_status.throttle_position_percent;
            msg.pt_compensation           = internal_combustion_engine_status.lambda_coefficient;

            mavlink_msg_efi_status_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // EFI_STATUS_HPP
