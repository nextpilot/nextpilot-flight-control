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
#ifndef EKF_MAG_H
#define EKF_MAG_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Mag : public Sensor {
public:
    Mag(std::shared_ptr<Ekf> ekf);
    ~Mag();

    void setData(const Vector3f &mag);

private:
    Vector3f _mag_data;

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // !EKF_MAG_H
