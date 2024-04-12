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
 * Feeds Ekf with airspeed data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_AIRSPEED_H
#define EKF_AIRSPEED_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Airspeed : public Sensor {
public:
    Airspeed(std::shared_ptr<Ekf> ekf);
    ~Airspeed();

    void setData(float true_airspeed, float eas2tas);

private:
    float _true_airspeed_data{0.0f};
    float _indicated_airspeed_data{0.0f};

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // !EKF_AIRSPEED_H
