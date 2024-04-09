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
 * Test code for the Field Sensor Bias Estimator
 * Run this test only using make tests TESTFILTER=FieldSensorBiasEstimator
 */

#include <gtest/gtest.h>
#include <FieldSensorBiasEstimator.hpp>

using namespace matrix;

TEST(MagnetometerBiasEstimatorTest, constantZRotation)
{
	FieldSensorBiasEstimator field_sensor_bias_estimator;
	field_sensor_bias_estimator.setLearningGain(10000.f);
	const Vector3f virtual_gyro = Vector3f(0.f, 0.f, 0.1f);
	Vector3f virtual_unbiased_mag = Vector3f(0.9f, 0.f, 1.79f); // taken from SITL jmavsim initialization
	const Vector3f virtual_bias(0.2f, -0.4f, 0.5f);
	Vector3f virtual_mag = virtual_unbiased_mag + virtual_bias;

	// Initialize with the current measurement
	field_sensor_bias_estimator.setField(virtual_mag);

	for (int i = 0; i <= 1000; i++) {
		float dt = .01f;

		// turn the mag values according to the gyro

		virtual_mag = virtual_unbiased_mag + virtual_bias;
		// printf("---- %i\n", i);
		// printf("virtual_gyro\n"); virtual_gyro.print();
		// printf("virtual_mag\n"); virtual_mag.print();

		field_sensor_bias_estimator.updateEstimate(virtual_gyro, virtual_mag, dt);
		virtual_unbiased_mag = Dcmf(AxisAnglef(-virtual_gyro * dt)) * virtual_unbiased_mag;
	}

	const Vector3f bias_est = field_sensor_bias_estimator.getBias();
	EXPECT_NEAR(bias_est(0), virtual_bias(0), 1e-3f) << "Estimated X bias " << bias_est(0);
	EXPECT_NEAR(bias_est(1), virtual_bias(1), 1e-3f) << "Estimated Y bias " << bias_est(1);
	// The Z bias is not observable due to pure yaw rotation
	EXPECT_NEAR(bias_est(2), 0.f, 1e-3f) << "Estimated Z bias " << bias_est(2);
}
