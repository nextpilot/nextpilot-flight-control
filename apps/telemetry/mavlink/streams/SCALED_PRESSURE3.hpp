/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef SCALED_PRESSURE3_HPP
#define SCALED_PRESSURE3_HPP

#include <uORB/topics/differential_pressure.h>
#include <uORB/topics/sensor_baro.h>

class MavlinkStreamScaledPressure3 : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamScaledPressure3(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "SCALED_PRESSURE3";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_SCALED_PRESSURE3;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_sensor_baro_sub.advertised() || _differential_pressure_sub.advertised()) {
            return MAVLINK_MSG_ID_SCALED_PRESSURE3_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamScaledPressure3(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _differential_pressure_sub{ORB_ID(differential_pressure), 2};
    uORB::Subscription _sensor_baro_sub{ORB_ID(sensor_baro), 2};

    bool send() override {
        if (_sensor_baro_sub.updated() || _differential_pressure_sub.updated()) {
            mavlink_scaled_pressure3_t msg{};

            sensor_baro_s sensor_baro;

            if (_sensor_baro_sub.copy(&sensor_baro)) {
                msg.time_boot_ms = sensor_baro.timestamp / 1000;
                msg.press_abs    = sensor_baro.pressure * 0.01f;            // Pa to hPa
                msg.temperature  = roundf(sensor_baro.temperature * 100.f); // cdegC (centidegrees)
            }

            differential_pressure_s differential_pressure;

            if (_differential_pressure_sub.copy(&differential_pressure)) {
                if (msg.time_boot_ms == 0) {
                    msg.time_boot_ms = differential_pressure.timestamp / 1000;
                }

                msg.press_diff             = differential_pressure.differential_pressure_pa * 0.01f; // Pa to hPa
                msg.temperature_press_diff = roundf(differential_pressure.temperature * 100.f);      // cdegC (centidegrees)
            }

            mavlink_msg_scaled_pressure3_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // SCALED_PRESSURE3_HPP
