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
#include "EKF/ekf.h"
#include "EKF/imu_down_sampler.hpp"

class EkfImuSamplingTest : public ::testing::TestWithParam<std::tuple<float, float, Vector3f, Vector3f>> {
public:
    Ekf _ekf{};

    uint32_t _t_us{0};

    // Setup the Ekf with synthetic measurements
    void SetUp() override {
        _ekf.init(0);
    }

    void TearDown() override {
    }
};

TEST_P(EkfImuSamplingTest, imuSamplingAtMultipleRates) {
    // WHEN: adding imu samples at a higher rate than the update loop
    // THEN: imu sample should be down sampled
    // WHEN: adding imu samples at a same or lower rate than the update loop
    // THEN: imu sample should reach buffer unchanged

    uint32_t dt_us          = std::get<0>(GetParam()) * (10 * 1000);
    uint32_t expected_dt_us = std::get<1>(GetParam()) * (10 * 1000);

    Vector3f  ang_vel = std::get<2>(GetParam());
    Vector3f  accel   = std::get<3>(GetParam());
    imuSample imu_sample;
    imu_sample.delta_ang_dt = dt_us * 1.0e-6f;
    imu_sample.delta_ang    = ang_vel * imu_sample.delta_ang_dt;
    imu_sample.delta_vel_dt = dt_us * 1.0e-6f;
    imu_sample.delta_vel    = accel * imu_sample.delta_vel_dt;

    // The higher the imu rate is the more measurements we have to set before reaching the FILTER_UPDATE_PERIOD
    int n_samples = 0;

    for (int i = 0; i < (int)30 / std::get<0>(GetParam()); ++i) {
        n_samples++;
        imu_sample.time_us = _t_us;
        _ekf.setIMUData(imu_sample);
        _t_us += dt_us;
    }

    // Get the imu sample that was put into the buffer
    imuSample imu_sample_buffered = _ekf.get_imu_sample_delayed();
    EXPECT_NEAR(expected_dt_us / 1e6f, imu_sample_buffered.delta_ang_dt, 1e-5f);
    EXPECT_NEAR(expected_dt_us / 1e6f, imu_sample_buffered.delta_vel_dt, 1e-5f);

    // WHEN: downsampling the imu measurement
    // THEN: the delta vel should be accumulated correctly
    // Allow for accumulation of rounding error with each sample
    EXPECT_TRUE(matrix::isEqual(ang_vel, imu_sample_buffered.delta_ang / imu_sample_buffered.delta_ang_dt,
                                float(n_samples) * 1e-7f));
    EXPECT_TRUE(matrix::isEqual(accel, imu_sample_buffered.delta_vel / imu_sample_buffered.delta_vel_dt,
                                float(n_samples) * 1e-7f));
}

INSTANTIATE_TEST_SUITE_P(imuSamplingAtMultipleRates,
                         EkfImuSamplingTest,
                         ::testing::Values(
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.0f, 1.0f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{-0.46f, 0.87f, 0.20f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.5f, 1.0f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{-0.46f, 0.87f, 0.20f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.6f, 1.6f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{-0.46f, 0.87f, 0.20f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.333f, 1.0f, Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{-0.46f, 0.87f, 0.20f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.0f, 1.0f, Vector3f{1.0f, 0.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.5f, 1.0f, Vector3f{1.0f, 0.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.6f, 1.6f, Vector3f{1.0f, 0.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.333f, 1.0f, Vector3f{1.0f, 0.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.0f, 1.0f, Vector3f{0.0f, 1.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.5f, 1.0f, Vector3f{0.0f, 1.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.6f, 1.6f, Vector3f{0.0f, 1.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.333f, 1.0f, Vector3f{0.0f, 1.0f, 0.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.0f, 1.0f, Vector3f{0.0f, 0.0f, 1.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.5f, 1.0f, Vector3f{0.0f, 0.0f, 1.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(1.6f, 1.6f, Vector3f{0.0f, 0.0f, 1.0f}, Vector3f{0.0f, 0.0f, 0.0f}),
                             std::make_tuple<float, float, Vector3f, Vector3f>(0.333f, 1.0f, Vector3f{0.0f, 0.0f, 1.0f}, Vector3f{0.0f, 0.0f, 0.0f})));

TEST_F(EkfImuSamplingTest, accelDownSampling) {
    int32_t        target_dt_us = 8000;
    ImuDownSampler sampler(target_dt_us);

    Vector3f  ang_vel{0.0f, 0.0f, 0.0f};
    Vector3f  accel{-0.46f, 0.87f, 0.0f};
    imuSample input_sample;
    input_sample.delta_ang_dt = 0.004f;
    input_sample.delta_ang    = ang_vel * input_sample.delta_ang_dt;
    input_sample.delta_vel_dt = 0.004f;
    input_sample.delta_vel    = accel * input_sample.delta_vel_dt;
    input_sample.time_us      = 0;

    // WHEN: adding samples at the double rate as the target rate
    EXPECT_FALSE(sampler.update(input_sample));
    input_sample.time_us = 4000;

    // THEN: after two samples a first downsampled sample is ready
    EXPECT_TRUE(sampler.update(input_sample));

    // THEN: downsampled sample should fit to input data
    imuSample output_sample = sampler.getDownSampledImuAndTriggerReset();
    EXPECT_FLOAT_EQ(output_sample.delta_ang_dt, 0.008f);
    EXPECT_FLOAT_EQ(output_sample.delta_vel_dt, 0.008f);
    EXPECT_TRUE(matrix::isEqual(ang_vel * 0.008f, output_sample.delta_ang, 1e-10f));
    EXPECT_TRUE(matrix::isEqual(accel * 0.008f, output_sample.delta_vel, 1e-10f));
}

TEST_F(EkfImuSamplingTest, gyroDownSampling) {
    int32_t        target_dt_us = 8000;
    ImuDownSampler sampler(target_dt_us);

    Vector3f  ang_vel{0.0f, 0.0f, 1.0f};
    Vector3f  accel{0.0f, 0.0f, 0.0f};
    imuSample input_sample;

    input_sample.delta_ang_dt = 0.004f;
    input_sample.delta_ang    = ang_vel * input_sample.delta_ang_dt;
    input_sample.delta_vel_dt = 0.004f;
    input_sample.delta_vel    = accel * input_sample.delta_vel_dt;
    input_sample.time_us      = 0;

    // WHEN: adding samples at the double rate as the target rate
    EXPECT_FALSE(sampler.update(input_sample));
    input_sample.time_us += 4000;

    // THEN: after two samples a first downsampled sample is ready
    EXPECT_TRUE(sampler.update(input_sample));
    input_sample.time_us += 4000;

    // THEN: downsampled sample should fit to input data
    imuSample output_sample = sampler.getDownSampledImuAndTriggerReset();
    EXPECT_FLOAT_EQ(output_sample.delta_ang_dt, 0.008f);
    EXPECT_FLOAT_EQ(output_sample.delta_vel_dt, 0.008f);
    EXPECT_TRUE(matrix::isEqual(ang_vel * 0.008f, output_sample.delta_ang, 1e-10f));
    EXPECT_TRUE(matrix::isEqual(accel * 0.008f, output_sample.delta_vel, 1e-10f));

    ang_vel                = Vector3f{0.0f, 1.0f, 0.0f};
    input_sample.delta_ang = ang_vel * input_sample.delta_ang_dt;
    input_sample.delta_vel = accel * input_sample.delta_vel_dt;

    // WHEN: adding samples at the double rate as the target rate
    EXPECT_FALSE(sampler.update(input_sample));
    input_sample.time_us += 4000;

    // THEN: after two more samples a second downsampled sample is ready
    EXPECT_TRUE(sampler.update(input_sample));
    input_sample.time_us += 4000;

    // THEN: downsampled sample should fit the adapted input data
    output_sample = sampler.getDownSampledImuAndTriggerReset();
    EXPECT_FLOAT_EQ(output_sample.delta_ang_dt, 0.008f);
    EXPECT_FLOAT_EQ(output_sample.delta_vel_dt, 0.008f);
    EXPECT_TRUE(matrix::isEqual(ang_vel * 0.008f, output_sample.delta_ang, 1e-10f));
    EXPECT_TRUE(matrix::isEqual(accel * 0.008f, output_sample.delta_vel, 1e-10f));

    ang_vel                = Vector3f{1.0f, 0.0f, 0.0f};
    input_sample.delta_ang = ang_vel * input_sample.delta_ang_dt;
    input_sample.delta_vel = accel * input_sample.delta_vel_dt;

    // WHEN: adding samples at the double rate as the target rate
    EXPECT_FALSE(sampler.update(input_sample));
    input_sample.time_us += 4000;

    // THEN: after two more samples a second downsampled sample is ready
    EXPECT_TRUE(sampler.update(input_sample));
    input_sample.time_us += 4000;

    // THEN: downsampled sample should fit the adapted input data
    output_sample = sampler.getDownSampledImuAndTriggerReset();
    EXPECT_FLOAT_EQ(output_sample.delta_ang_dt, 0.008f);
    EXPECT_FLOAT_EQ(output_sample.delta_vel_dt, 0.008f);
    EXPECT_TRUE(matrix::isEqual(ang_vel * 0.008f, output_sample.delta_ang, 1e-10f));
    EXPECT_TRUE(matrix::isEqual(accel * 0.008f, output_sample.delta_vel, 1e-10f));
}
