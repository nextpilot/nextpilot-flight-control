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
 * Base class for defining the interface for simulaton of a sensor
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */

#ifndef EKF_SENSOR_H
#define EKF_SENSOR_H

#include "EKF/ekf.h"
#include <math.h>
#include <memory>

namespace sensor_simulator {

class Sensor {
public:
    Sensor(std::shared_ptr<Ekf> ekf);
    virtual ~Sensor();

    void update(uint64_t time);

    void setRateHz(uint32_t rate) {
        _update_period = uint32_t(1000000) / rate;
    }

    bool isRunning() const {
        return _is_running;
    }

    void start() {
        _is_running = true;
    }

    void stop() {
        _is_running = false;
    }

    bool should_send(uint64_t time) const;

protected:
    std::shared_ptr<Ekf> _ekf;
    // time in microseconds
    uint32_t _update_period;
    uint64_t _time_last_data_sent{0};

    bool _is_running{false};

    // Checks that the right amount time passed since last send data to fulfill rate
    bool is_time_to_send(uint64_t time) const;

    // call set*Data function of Ekf
    virtual void send(uint64_t time) = 0;
};

} // namespace sensor_simulator
#endif // !EKF_SENSOR_H
