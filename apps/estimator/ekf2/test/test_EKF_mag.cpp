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
 * Test the mag fusion
 */

#include <gtest/gtest.h>
#include "EKF/ekf.h"
#include "sensor_simulator/sensor_simulator.h"
#include "sensor_simulator/ekf_wrapper.h"
#include "test_helper/reset_logging_checker.h"

class EkfMagTest : public ::testing::Test {
public:
    EkfMagTest() :
        ::testing::Test(),
        _ekf{std::make_shared<Ekf>()},
        _sensor_simulator(_ekf),
        _ekf_wrapper(_ekf){};

    std::shared_ptr<Ekf> _ekf;
    SensorSimulator      _sensor_simulator;
    EkfWrapper           _ekf_wrapper;

    // Setup the Ekf with synthetic measurements
    void SetUp() override {
        // run briefly to init, then manually set in air and at rest (default for a real vehicle)
        _ekf->init(0);
        _sensor_simulator.runSeconds(0.1);
        _ekf->set_in_air_status(false);
        _ekf->set_vehicle_at_rest(true);
    }

    const uint32_t _init_duration_s{6};
};

TEST_F(EkfMagTest, fusionStartWithReset) {
    // GIVEN: some meaningful mag data
    const float    mag_heading = M_PI_F / 3.f;
    const Vector3f mag_data(0.2f * cosf(mag_heading), -0.2f * sinf(mag_heading), 0.4f);
    _sensor_simulator._mag.setData(mag_data);

    const int initial_quat_reset_counter = _ekf_wrapper.getQuaternionResetCounter();
    _sensor_simulator.runSeconds(_init_duration_s);

    // THEN: the fusion initializes using the mag data and runs normally
    EXPECT_NEAR(_ekf_wrapper.getYawAngle(), mag_heading, radians(1.f));
    EXPECT_TRUE(_ekf_wrapper.isIntendingMagHeadingFusion());
    EXPECT_FALSE(_ekf_wrapper.isIntendingMag3DFusion());

    EXPECT_EQ(_ekf_wrapper.getQuaternionResetCounter(), initial_quat_reset_counter + 1);
}

TEST_F(EkfMagTest, noInitLargeStrength) {
    // GIVEN: a really large magnetic field
    _ekf_wrapper.enableMagStrengthCheck();
    const Vector3f mag_data(1.f, 1.f, 1.f);
    _sensor_simulator._mag.setData(mag_data);

    const int initial_quat_reset_counter = _ekf_wrapper.getQuaternionResetCounter();
    _sensor_simulator.runSeconds(_init_duration_s);

    // THEN: the fusion shouldn't start
    EXPECT_FALSE(_ekf_wrapper.isIntendingMagHeadingFusion());
    EXPECT_FALSE(_ekf_wrapper.isIntendingMag3DFusion());
    EXPECT_EQ(0, (int)_ekf->control_status_flags().yaw_align);
    EXPECT_EQ(_ekf_wrapper.getQuaternionResetCounter(), initial_quat_reset_counter);
}

TEST_F(EkfMagTest, suddenLargeStrength) {
    _ekf_wrapper.enableMagStrengthCheck();

    // GIVEN: some meaningful mag data
    const float mag_heading = -M_PI_F / 7.f;
    Vector3f    mag_data(0.2f * cosf(mag_heading), -0.2f * sinf(mag_heading), 0.4f);
    _sensor_simulator._mag.setData(mag_data);

    _sensor_simulator.runSeconds(_init_duration_s);

    // THEN: the fusion initializes using the mag data and runs normally
    EXPECT_NEAR(_ekf_wrapper.getYawAngle(), mag_heading, radians(1.f));
    EXPECT_TRUE(_ekf_wrapper.isIntendingMagHeadingFusion());
    EXPECT_FALSE(_ekf_wrapper.isIntendingMag3DFusion());

    // BUT WHEN: the mag field norm is suddenly too large
    mag_data *= 5.f;
    _sensor_simulator._mag.setData(mag_data);
    _sensor_simulator.runSeconds(6.f);

    // THEN: the mag fusion should stop after some time
    EXPECT_FALSE(_ekf_wrapper.isIntendingMagHeadingFusion());
    EXPECT_FALSE(_ekf_wrapper.isIntendingMag3DFusion());
}
