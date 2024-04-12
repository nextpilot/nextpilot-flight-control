/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * Feeds Ekf with Imu data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_IMU_H
#define EKF_IMU_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Imu : public Sensor {
public:
    Imu(std::shared_ptr<Ekf> ekf);
    ~Imu();

    void setData(const Vector3f &accel, const Vector3f &gyro);
    void setAccelData(const Vector3f &accel);
    void setGyroData(const Vector3f &gyro);
    void setAccelClipping(bool x, bool y, bool z);

    bool moving() {
        return ((fabsf(_accel_data.norm() - CONSTANTS_ONE_G) > 0.01f) || (_gyro_data.norm() > 0.01f));
    }

private:
    Vector3f _accel_data;
    Vector3f _gyro_data;
    bool     _is_accel_clipping[3]{};

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // EKF_IMU_H
