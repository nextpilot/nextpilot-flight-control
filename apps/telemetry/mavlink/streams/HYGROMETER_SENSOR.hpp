/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef HYGROMETER_SENSOR_HPP
#define HYGROMETER_SENSOR_HPP

#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/topics/sensor_hygrometer.h>

class MavlinkStreamHygrometerSensor : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamHygrometerSensor(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "HYGROMETER_SENSOR";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_HYGROMETER_SENSOR;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _sensor_hygrometer_subs.advertised_count() * (MAVLINK_MSG_ID_HYGROMETER_SENSOR_LEN +
                                                             MAVLINK_NUM_NON_PAYLOAD_BYTES);
    }

private:
    explicit MavlinkStreamHygrometerSensor(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::SubscriptionMultiArray<sensor_hygrometer_s> _sensor_hygrometer_subs{ORB_ID::sensor_hygrometer};

    bool send() override {
        bool updated = false;

        for (int i = 0; i < _sensor_hygrometer_subs.size(); i++) {
            sensor_hygrometer_s sensor_hygrometer;

            if (_sensor_hygrometer_subs[i].update(&sensor_hygrometer)) {
                mavlink_hygrometer_sensor_t msg{};
                msg.id          = i;                                             // uint8_t Hygrometer ID
                msg.temperature = roundf(sensor_hygrometer.temperature * 100.f); // degrees to centidegrees (int16_t)
                msg.humidity    = roundf(sensor_hygrometer.humidity);            // % (uint16_t)

                mavlink_msg_hygrometer_sensor_send_struct(_mavlink->get_channel(), &msg);

                updated = true;
            }
        }

        return updated;
    }
};
#endif // HYGROMETER_SENSOR_HPP
