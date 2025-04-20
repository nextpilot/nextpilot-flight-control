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
#include <random>
#include <matrix/matrix/math.hpp>
#include "WelfordMeanVector.hpp"

using namespace math;
using matrix::Vector3f;

TEST(WelfordMeanVectorTest, NoisySignal)
{
	const float std_dev = 3.f;
	std::normal_distribution<float> standard_normal_distribution{0.f, std_dev};
	std::default_random_engine random_generator{}; // Pseudo-random generator with constant seed
	random_generator.seed(42);
	WelfordMeanVector<float, 3> welford{};

	for (int i = 0; i < 1000; i++) {
		const float noisy_value = standard_normal_distribution(random_generator);
		welford.update(Vector3f(noisy_value, noisy_value - 1.f, noisy_value + 1.f));
	}

	EXPECT_TRUE(welford.valid());
	const Vector3f mean = welford.mean();
	const Vector3f var = welford.variance();

	const float var_real = std_dev * std_dev;

	EXPECT_NEAR(mean(0), 0.f, 0.7f);
	EXPECT_NEAR(mean(1), -1.f, 0.7f);
	EXPECT_NEAR(mean(2), 1.f, 0.7f);
	EXPECT_NEAR(var(0), var_real, 0.1f);
	EXPECT_NEAR(var(1), var_real, 0.1f);
	EXPECT_NEAR(var(2), var_real, 0.1f);
}
