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
 * Test the fusion of body frame specific forces for the estimation of wind speed
 */

#include <gtest/gtest.h>
#include "EKF/ekf.h"
#include "sensor_simulator/sensor_simulator.h"
#include "sensor_simulator/ekf_wrapper.h"

class EkfDragFusionTest : public ::testing::Test {
public:
    EkfDragFusionTest() :
        ::testing::Test(),
        _ekf{std::make_shared<Ekf>()},
        _sensor_simulator(_ekf),
        _ekf_wrapper(_ekf),
        _quat_sim(Eulerf(0.0f, 0.0f, 0.0f)){};

    std::shared_ptr<Ekf> _ekf;
    SensorSimulator      _sensor_simulator;
    EkfWrapper           _ekf_wrapper;
    const Quatf          _quat_sim;

    // Setup the Ekf with synthetic measurements
    void SetUp() override {
        // run briefly to init, then manually set in air and at rest (default for a real vehicle)
        _ekf->init(0);
        _ekf->set_is_fixed_wing(false);
        _ekf->set_in_air_status(false);
        _ekf->set_vehicle_at_rest(true);
    }

    // Use this method to clean up any memory, network etc. after each test
    void TearDown() override {
    }
};

TEST_F(EkfDragFusionTest, testForwardMomentumDrag) {
    const float  pitch = math::radians(10.0f);
    const float  roll  = math::radians(0.0f);
    const Eulerf euler_angles_sim(roll, pitch, 0.0f);
    const Quatf  quat_sim(euler_angles_sim);
    _sensor_simulator.simulateOrientation(quat_sim);

    _ekf_wrapper.enableGpsFusion();
    _sensor_simulator.startGps();

    const float bcoef_x = 0.0f;
    const float bcoef_y = 0.0f;
    const float mcoef   = 0.15f;
    _ekf_wrapper.setDragFusionParameters(bcoef_x, bcoef_y, mcoef);
    _ekf_wrapper.enableDragFusion();

    // simulate a vehicle that is hovering and tilting into wind

    _ekf->set_in_air_status(true);
    _ekf->set_vehicle_at_rest(false);

    // Wind estimation is slow when using drag fusion
    _sensor_simulator.runSeconds(90);

    EXPECT_TRUE(_ekf_wrapper.isWindVelocityEstimated());

    const Vector2f vel_wind_earth = _ekf->getWindVelocity();

    // drag acceleration = mcoef * airspeed
    Vector2f predicted_accel;
    predicted_accel(0)  = CONSTANTS_ONE_G * sinf(pitch);
    predicted_accel(1)  = -CONSTANTS_ONE_G * sinf(roll);
    Vector2f wind_speed = predicted_accel / mcoef;
    EXPECT_NEAR(vel_wind_earth(0), wind_speed(0), fmaxf(1.0f, 0.1f * fabsf(wind_speed(0))));
    EXPECT_NEAR(vel_wind_earth(1), wind_speed(1), fmaxf(1.0f, 0.1f * fabsf(wind_speed(1))));
};

TEST_F(EkfDragFusionTest, testLateralMomentumDrag) {
    const float  pitch = math::radians(0.0f);
    const float  roll  = math::radians(10.0f);
    const Eulerf euler_angles_sim(roll, pitch, 0.0f);
    const Quatf  quat_sim(euler_angles_sim);
    _sensor_simulator.simulateOrientation(quat_sim);

    _ekf_wrapper.enableGpsFusion();
    _sensor_simulator.startGps();

    // Apply parameter changes required to do drag fusion wind estimation
    const float bcoef_x = 0.0f;
    const float bcoef_y = 0.0f;
    const float mcoef   = 0.15f;
    _ekf_wrapper.setDragFusionParameters(bcoef_x, bcoef_y, mcoef);
    _ekf_wrapper.enableDragFusion();

    // simulate a vehicle that is hovering and tilting into wind

    _ekf->set_in_air_status(true);
    _ekf->set_vehicle_at_rest(false);

    // Wind estimation is slow when using drag fusion
    _sensor_simulator.runSeconds(90);

    EXPECT_TRUE(_ekf_wrapper.isWindVelocityEstimated());

    const Vector2f vel_wind_earth = _ekf->getWindVelocity();

    // drag acceleration = mcoef * airspeed
    Vector2f predicted_accel;
    predicted_accel(0)  = CONSTANTS_ONE_G * sinf(pitch);
    predicted_accel(1)  = -CONSTANTS_ONE_G * sinf(roll);
    Vector2f wind_speed = predicted_accel / mcoef;
    EXPECT_NEAR(vel_wind_earth(0), wind_speed(0), fmaxf(1.0f, 0.1f * fabsf(wind_speed(0))));
    EXPECT_NEAR(vel_wind_earth(1), wind_speed(1), fmaxf(1.0f, 0.1f * fabsf(wind_speed(1))));
};

TEST_F(EkfDragFusionTest, testForwardBluffBodyDrag) {
    const float  roll  = math::radians(0.0f);
    const float  pitch = math::radians(-10.0f);
    const Eulerf euler_angles_sim(roll, pitch, 0.0f);
    const Quatf  quat_sim(euler_angles_sim);
    _sensor_simulator.simulateOrientation(quat_sim);

    _ekf_wrapper.enableGpsFusion();
    _sensor_simulator.startGps();

    // Apply parameter changes required to do drag fusion wind estimation
    const float bcoef_x = 70.0f;
    const float bcoef_y = 50.0f;
    const float mcoef   = 0.0f;
    _ekf_wrapper.setDragFusionParameters(bcoef_x, bcoef_y, mcoef);
    _ekf_wrapper.enableDragFusion();

    // simulate a vehicle that is hovering and tilting into wind

    _ekf->set_in_air_status(true);
    _ekf->set_vehicle_at_rest(false);

    // Wind estimation is slow when using drag fusion
    _sensor_simulator.runSeconds(90);

    EXPECT_TRUE(_ekf_wrapper.isWindVelocityEstimated());

    const Vector2f vel_wind_earth = _ekf->getWindVelocity();

    Vector2f    predicted_accel(CONSTANTS_ONE_G * sinf(pitch), 0.0f);
    const float airspeed = sqrtf((2.0f * bcoef_x * predicted_accel.length()) /
                                 CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
    Vector2f    wind_speed(-airspeed, 0.0f);

    // The magnitude of error perpendicular to wind is equivalent to the error in the direction of wind
    // which is why we use the same threshold for each axis.
    EXPECT_NEAR(vel_wind_earth(0), wind_speed(0), 1.0f);
    EXPECT_NEAR(vel_wind_earth(1), wind_speed(1), 1.0f);
};

TEST_F(EkfDragFusionTest, testLateralBluffBodyDrag) {
    const float  roll  = math::radians(10.0f);
    const float  pitch = math::radians(0.0f);
    const Eulerf euler_angles_sim(roll, pitch, 0.0f);
    const Quatf  quat_sim(euler_angles_sim);
    _sensor_simulator.simulateOrientation(quat_sim);

    _ekf_wrapper.enableGpsFusion();
    _sensor_simulator.startGps();

    // Apply parameter changes required to do drag fusion wind estimation
    const float bcoef_x = 70.0f;
    const float bcoef_y = 50.0f;
    const float mcoef   = 0.0f;
    _ekf_wrapper.setDragFusionParameters(bcoef_x, bcoef_y, mcoef);
    _ekf_wrapper.enableDragFusion();

    // simulate a vehicle that is hovering and tilting into wind
    _ekf->set_in_air_status(true);
    _ekf->set_vehicle_at_rest(false);

    // Wind estimation is slow when using drag fusion
    _sensor_simulator.runSeconds(90);

    EXPECT_TRUE(_ekf_wrapper.isWindVelocityEstimated());

    const Vector2f vel_wind_earth = _ekf->getWindVelocity();

    Vector2f    predicted_accel(0.0f, -CONSTANTS_ONE_G * sinf(roll));
    const float airspeed = sqrtf((2.0f * bcoef_y * predicted_accel.length()) /
                                 CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
    Vector2f    wind_speed(0.0f, -airspeed);

    // The magnitude of error perpendicular to wind is equivalent to the error in the of wind
    // which is why we use the same threshold for each axis.
    EXPECT_NEAR(vel_wind_earth(0), wind_speed(0), 1.0f);
    EXPECT_NEAR(vel_wind_earth(1), wind_speed(1), 1.0f);
};

TEST_F(EkfDragFusionTest, testDiagonalBluffBodyDrag) {
    const float  roll  = math::radians(-10.0f);
    const float  pitch = math::radians(-10.0f);
    const Eulerf euler_angles_sim(roll, pitch, 0.f);
    const Quatf  quat_sim(euler_angles_sim);
    _sensor_simulator.simulateOrientation(quat_sim);

    _ekf_wrapper.enableGpsFusion();
    _sensor_simulator.startGps();

    // Apply parameter changes required to do drag fusion wind estimation
    const float bcoef_x = 50.0f;
    const float bcoef_y = 50.0f;
    const float mcoef   = 0.0f;
    _ekf_wrapper.setDragFusionParameters(bcoef_x, bcoef_y, mcoef);
    _ekf_wrapper.enableDragFusion();

    // simulate a vehicle that is hovering and tilting into wind
    _ekf->set_in_air_status(true);
    _ekf->set_vehicle_at_rest(false);

    // Wind estimation is slow when using drag fusion
    _sensor_simulator.runSeconds(90);

    EXPECT_TRUE(_ekf_wrapper.isWindVelocityEstimated());

    const Vector2f vel_wind_earth = _ekf->getWindVelocity();

    Vector2f    predicted_accel = quat_sim.rotateVectorInverse(Vector3f(0.f, 0.f, -CONSTANTS_ONE_G)).xy();
    const float airspeed        = sqrtf((2.0f * bcoef_y * predicted_accel.norm()) /
                                        CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C);
    Vector2f    wind_speed(airspeed * predicted_accel / predicted_accel.norm());

    // The magnitude of error perpendicular to wind is equivalent to the error in the of wind
    // which is why we use the same threshold for each axis.
    EXPECT_NEAR(vel_wind_earth(0), wind_speed(0), 1.0f);
    EXPECT_NEAR(vel_wind_earth(1), wind_speed(1), 1.0f);
};
