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
 * Feeds Ekf with Mag data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_BARO_H
#define EKF_BARO_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Baro : public Sensor {
public:
    Baro(std::shared_ptr<Ekf> ekf);
    ~Baro();

    void  setData(float baro);
    float getData() const {
        return _baro_data;
    }

private:
    float _baro_data{0.0f};

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // !EKF_BARO_H
