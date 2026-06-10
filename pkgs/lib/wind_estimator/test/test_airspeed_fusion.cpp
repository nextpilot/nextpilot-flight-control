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
 * Run this test only using make tests TESTFILTER=unit-test_airspeed_fusion
 */

#include <gtest/gtest.h>
#include <matrix/math.hpp>
#include <mathlib/math/Functions.hpp>
#include "../python/generated/fuse_airspeed.h"

using namespace matrix;

TEST(TestAirspeedFusion, SingularityHandling)
{
	// GIVEN: all inputs set to zero (singularity point)
	const float true_airspeed = 0.f;
	const Vector3f vel_i{0.f, 0.f, 0.f};
	const Vector3f state{0.f, 0.f, 0.f};
	const Matrix<float, 3, 3> P;
	const float tas_var = 0.f;

	matrix::Matrix<float, 1, 3> H;
	matrix::Matrix<float, 3, 1> K;
	float innov_var;
	float innov;

	// WHEN: no singularity handling is done
	float epsilon = 0.f;
	sym::FuseAirspeed(vel_i, state, P, true_airspeed, tas_var, epsilon,
			  &H, &K, &innov_var, &innov);

	// THEN: some of the returned values are not finite
	EXPECT_TRUE(std::isnan(H(0, 0))) << H(0, 0);
	EXPECT_TRUE(std::isnan(H(0, 1))) << H(0, 1);
	EXPECT_FLOAT_EQ(H(0, 2), 0.f) << H(0, 2);
	EXPECT_TRUE(std::isnan(K(0, 0))) << K(0, 0);
	EXPECT_TRUE(std::isnan(K(1, 0))) << K(1, 0);
	EXPECT_TRUE(std::isnan(K(2, 0))) << K(2, 0);
	EXPECT_TRUE(std::isnan(innov_var));
	EXPECT_FLOAT_EQ(innov, 0.f);

	// BUT WHEN: singularity is avoided by addind a small constant
	epsilon = FLT_EPSILON;
	sym::FuseAirspeed(vel_i, state, P, true_airspeed, tas_var, epsilon,
			  &H, &K, &innov_var, &innov);

	// THEN: the returned values are finite and the Kalman gain is null
	EXPECT_FLOAT_EQ(H(0, 0), 0.f) << H(0, 0);
	EXPECT_FLOAT_EQ(H(0, 1), 0.f) << H(0, 1);
	EXPECT_FLOAT_EQ(H(0, 2), std::sqrt(FLT_EPSILON)) << H(0, 2);
	EXPECT_TRUE(isEqual(K, Vector3f()));
	EXPECT_FLOAT_EQ(innov_var, tas_var);
	EXPECT_FLOAT_EQ(innov, 0.f);
}
