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
 * This class is providing methods to feed the ECL EKF with measurement.
 * It takes a pointer to the Ekf object and will manipulate the object
 * by call set*Data functions.
 * It simulates the time to allow for sensor data being set at certain rate
 * and also calls the update method of the EKF
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */

#ifndef EKF_SENSOR_SIMULATOR_H
#define EKF_SENSOR_SIMULATOR_H

#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <motion_planning/VelocitySmoothing.hpp>

#include "imu.h"
#include "mag.h"
#include "baro.h"
#include "gps.h"
#include "flow.h"
#include "range_finder.h"
#include "vio.h"
#include "airspeed.h"
#include "EKF/ekf.h"

using namespace sensor_simulator::sensor;

struct sensor_info {
    uint64_t timestamp{};
    enum class measurement_t { IMU,
                               MAG,
                               BARO,
                               GPS,
                               AIRSPEED,
                               RANGE,
                               FLOW,
                               VISION,
                               LANDING_STATUS } sensor_type =
        measurement_t::IMU;
    std::array<double, 10> sensor_data{};
};

class SensorSimulator {
public:
    SensorSimulator(std::shared_ptr<Ekf> ekf);
    ~SensorSimulator() = default;

    uint64_t getTime() const {
        return _time;
    };

    void runSeconds(float duration_seconds);
    void runMicroseconds(uint32_t duration);

    void runReplaySeconds(float duration_seconds);
    void runReplayMicroseconds(uint32_t duration);

    void setTrajectoryTargetVelocity(const Vector3f &velocity_target);
    void runTrajectorySeconds(float duration_seconds);
    void runTrajectoryMicroseconds(uint32_t duration);

    void startBaro() {
        _baro.start();
    }
    void stopBaro() {
        _baro.stop();
    }

    void startGps() {
        _gps.start();
    }
    void stopGps() {
        _gps.stop();
    }

    void startFlow() {
        _flow.start();
    }
    void stopFlow() {
        _flow.stop();
    }

    void startRangeFinder() {
        _rng.start();
    }
    void stopRangeFinder() {
        _rng.stop();
    }

    void startExternalVision() {
        _vio.start();
    }
    void stopExternalVision() {
        _vio.stop();
    }

    void startAirspeedSensor() {
        _airspeed.start();
    }
    void stopAirspeedSensor() {
        _airspeed.stop();
    }

    void setGpsLatitude(const double latitude);
    void setGpsLongitude(const double longitude);
    void setGpsAltitude(const float altitude);

    void setImuBias(Vector3f accel_bias, Vector3f gyro_bias);

    void simulateOrientation(Quatf orientation);
    void setOrientation(const Quatf &orientation) {
        _R_body_to_world = Dcmf(orientation);
    }
    void setOrientation(const Dcmf &orientation) {
        _R_body_to_world = orientation;
    }

    void loadSensorDataFromFile(std::string filename);

    Airspeed    _airspeed;
    Baro        _baro;
    Flow        _flow;
    Gps         _gps;
    Imu         _imu;
    Mag         _mag;
    RangeFinder _rng;
    Vio         _vio;

    VelocitySmoothing _trajectory[3];

private:
    void setSensorDataToDefault();
    void setSensorDataFromReplayData();
    void setSensorRateToDefault();
    void setSingleReplaySample(const sensor_info &sample);
    void setSensorDataFromTrajectory();
    void startBasicSensor();
    void updateSensors();

    std::shared_ptr<Ekf> _ekf{nullptr};

    std::vector<sensor_info> _replay_data{};

    bool _has_replay_data{false};

    uint64_t _current_replay_data_index{0};
    uint64_t _time{0}; // microseconds

    Dcmf _R_body_to_world{};
};
#endif // !EKF_SENSOR_SIMULATOR_H
