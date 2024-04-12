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
 * Feeds Ekf with range finder data
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_RANGE_FINDER_H
#define EKF_RANGE_FINDER_H

#include "sensor.h"

namespace sensor_simulator {
namespace sensor {

class RangeFinder : public Sensor {
public:
    RangeFinder(std::shared_ptr<Ekf> ekf);
    ~RangeFinder();

    void setData(float range_data, int8_t range_quality);
    void setLimits(float min_distance_m, float max_distance_m);

private:
    rangeSample _range_sample{};
    float       _min_distance{0.2f};
    float       _max_distance{20.0f};

    void send(uint64_t time) override;
};

}
} // namespace sensor_simulator::sensor
#endif // !EKF_RANGE_FINDER_H
