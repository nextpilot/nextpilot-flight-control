/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include <math.h>
#include <memory>
#include "EKF/ekf.h"
#include "sensor_simulator/sensor_simulator.h"
#include "sensor_simulator/ekf_wrapper.h"
#include "sensor_simulator/ekf_logger.h"

class EkfReplayTest : public ::testing::Test {
public:
    EkfReplayTest() :
        ::testing::Test(),
        _ekf{std::make_shared<Ekf>()},
        _sensor_simulator(_ekf),
        _ekf_wrapper(_ekf),
        _ekf_logger(_ekf){};

    std::shared_ptr<Ekf> _ekf;
    SensorSimulator      _sensor_simulator;
    EkfWrapper           _ekf_wrapper;
    EkfLogger            _ekf_logger;
};

TEST_F(EkfReplayTest, irisGps) {
    _sensor_simulator.loadSensorDataFromFile(TEST_DATA_PATH "/replay_data/iris_gps.csv");
    _ekf_logger.setFilePath(TEST_DATA_PATH "/change_indication/iris_gps.csv");

    // Start simulation and enable fusion of additional sensor types here
    // By default the IMU, Baro and Mag sensor simulators are already running
    _sensor_simulator.startGps();
    _ekf_wrapper.enableGpsFusion();

    uint8_t logging_rate_hz = 10;

    for (int i = 0; i < 35 * logging_rate_hz; ++i) {
        _sensor_simulator.runReplaySeconds(1.0f / logging_rate_hz);
        _ekf_logger.writeStateToFile();
    }
}

TEST_F(EkfReplayTest, ekfGsfReset) {
    _sensor_simulator.loadSensorDataFromFile(TEST_DATA_PATH "/replay_data/ekf_gsf_reset.csv");
    _ekf_logger.setFilePath(TEST_DATA_PATH "/change_indication/ekf_gsf_reset.csv");

    // Start simulation and enable fusion of additional sensor types here
    // By default the IMU, Baro and Mag sensor simulators are already running
    _sensor_simulator.startGps();
    _ekf_wrapper.enableGpsFusion();
    auto params                = _ekf->getParamHandle();
    params->gps_vel_innov_gate = 1.f;
    params->gps_pos_innov_gate = 1.f;

    uint8_t logging_rate_hz = 10;

    for (int i = 0; i < 39 * logging_rate_hz; ++i) {
        _sensor_simulator.runReplaySeconds(1.0f / logging_rate_hz);
        _ekf_logger.writeStateToFile();
    }
}
