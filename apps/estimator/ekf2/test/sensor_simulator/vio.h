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
 * Feeds Ekf external vision data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_VIO_H
#define EKF_VIO_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Vio : public Sensor {
public:
    Vio(std::shared_ptr<Ekf> ekf);
    ~Vio();

    void setData(const extVisionSample &vio_data);
    void setVelocityVariance(const Vector3f &velVar);
    void setPositionVariance(const Vector3f &posVar);
    void setAngularVariance(float angVar);
    void setVelocity(const Vector3f &vel);
    void setPosition(const Vector3f &pos);
    void setOrientation(const Quatf &quat);

    void setVelocityFrameToLocalNED();
    void setVelocityFrameToLocalFRD();
    void setVelocityFrameToBody();

    void setPositionFrameToLocalNED();
    void setPositionFrameToLocalFRD();

    extVisionSample dataAtRest();

private:
    extVisionSample _vio_data;

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // !EKF_VIO_H
