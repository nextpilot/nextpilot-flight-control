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
 * Feeds Ekf with optical flow data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_FLOW_H
#define EKF_FLOW_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class Flow : public Sensor {
public:
    Flow(std::shared_ptr<Ekf> ekf);
    ~Flow();

    void       setData(const flowSample &flow);
    flowSample dataAtRest();

private:
    flowSample _flow_data;

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // !EKF_FLOW_H
