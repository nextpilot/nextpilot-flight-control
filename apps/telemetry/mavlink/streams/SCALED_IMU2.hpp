/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef SCALED_IMU2_HPP
#define SCALED_IMU2_HPP

#include <geo/geo.h>
#include <matrix/matrix/math.hpp>

#include <uORB/topics/sensor_mag.h>
#include <uORB/topics/vehicle_imu.h>
#include <uORB/topics/vehicle_imu_status.h>

class MavlinkStreamScaledIMU2 : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamScaledIMU2(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "SCALED_IMU2";
    }

    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_SCALED_IMU2;
    }

    const char *get_name() const override {
        return get_name_static();
    }

    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        if (_vehicle_imu_sub.advertised() || _sensor_mag_sub.advertised()) {
            return MAVLINK_MSG_ID_SCALED_IMU2_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        }

        return 0;
    }

private:
    explicit MavlinkStreamScaledIMU2(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_imu_sub{ORB_ID(vehicle_imu), 1};
    uORB::Subscription _vehicle_imu_status_sub{ORB_ID(vehicle_imu_status), 1};
    uORB::Subscription _sensor_mag_sub{ORB_ID(sensor_mag), 1};

    bool send() override {
        if (_vehicle_imu_sub.updated() || _sensor_mag_sub.updated()) {
            mavlink_scaled_imu2_t msg{};

            float temperature = NAN;

            vehicle_imu_s imu;

            if (_vehicle_imu_sub.copy(&imu)) {
                msg.time_boot_ms = imu.timestamp / 1000;

                // Accelerometer in mG
                const float    accel_dt_inv = 1.e6f / (float)imu.delta_velocity_dt;
                const Vector3f accel        = Vector3f{imu.delta_velocity} * accel_dt_inv * 1000.0f / CONSTANTS_ONE_G;
                msg.xacc                    = (int16_t)accel(0);
                msg.yacc                    = (int16_t)accel(1);
                msg.zacc                    = (int16_t)accel(2);

                // Gyroscope in mrad/s
                const float    gyro_dt_inv = 1.e6f / (float)imu.delta_angle_dt;
                const Vector3f gyro        = Vector3f{imu.delta_angle} * gyro_dt_inv * 1000.0f;
                msg.xgyro                  = gyro(0);
                msg.ygyro                  = gyro(1);
                msg.zgyro                  = gyro(2);

                // IMU temperature
                vehicle_imu_status_s vehicle_imu_status;

                if (_vehicle_imu_status_sub.copy(&vehicle_imu_status)) {
                    if (PX4_ISFINITE(vehicle_imu_status.temperature_accel)) {
                        temperature = vehicle_imu_status.temperature_accel;

                    } else if (PX4_ISFINITE(vehicle_imu_status.temperature_gyro)) {
                        temperature = vehicle_imu_status.temperature_gyro;
                    }
                }
            }

            sensor_mag_s sensor_mag;

            if (_sensor_mag_sub.copy(&sensor_mag)) {
                if (msg.time_boot_ms == 0) {
                    msg.time_boot_ms = sensor_mag.timestamp / 1000;
                }

                msg.xmag = sensor_mag.x * 1000.0f; // Gauss -> MilliGauss
                msg.ymag = sensor_mag.y * 1000.0f; // Gauss -> MilliGauss
                msg.zmag = sensor_mag.z * 1000.0f; // Gauss -> MilliGauss

                if (!PX4_ISFINITE(temperature) && PX4_ISFINITE(sensor_mag.temperature)) {
                    temperature = sensor_mag.temperature;
                }
            }

            if (PX4_ISFINITE(temperature)) {
                // degrees -> centi-degrees constrained to int16
                msg.temperature = math::constrain(roundf(temperature * 100.f), (float)INT16_MIN, (float)INT16_MAX);

                if (msg.temperature == 0) {
                    // if the IMU is at 0C it must send 1 (0.01C).
                    msg.temperature = 1;
                }
            }

            mavlink_msg_scaled_imu2_send_struct(_mavlink->get_channel(), &msg);
            return true;
        }

        return false;
    }
};
#endif /* SCALED_IMU2_HPP */
