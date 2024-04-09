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
#include "SlewRateYaw.hpp"

TEST(SlewRateYawTest, SlewUpLimited)
{
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(.15f);
	_slew_rate_yaw.setForcedValue(1.1f);

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate_yaw.update(3.f, 1.f), 1.1f + i * .15f);
	}
}

TEST(SlewRateYawTest, SlewDownLimited)
{
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(.1f);
	_slew_rate_yaw.setForcedValue(.5f);

	for (int i = 1; i <= 10; i++) {
		EXPECT_NEAR(_slew_rate_yaw.update(-2.f, 1.f), .5f - i * .1f, 1e-7f);
	}
}

TEST(SlewRateYawTest, ReachValueSlewed)
{
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(.2f);
	_slew_rate_yaw.setForcedValue(1.f);

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate_yaw.update(3.f, 1.f), 1.f + i * .2f);
	}

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate_yaw.update(3.f, 1.f), 3.f);
	}
}

TEST(SlewRateYawTest, SlewUpWrappedOutput)
{
	// put the goal value always a bit further away such that at some point the output has to wrap
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(.2f);
	_slew_rate_yaw.setForcedValue(0.f);

	for (int i = 1; i <= 30; i++) {
		EXPECT_NEAR(_slew_rate_yaw.update(i * .25f, 1.f), matrix::wrap_pi(i * .2f), 1e-5f);
	}
}

TEST(SlewRateYawTest, SlewDownWrappedOutput)
{
	// put the goal value always a bit further away such that at some point the output has to wrap
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(.2f);
	_slew_rate_yaw.setForcedValue(0.f);

	for (int i = 1; i <= 50; i++) {
		EXPECT_NEAR(_slew_rate_yaw.update(i * -.25f, 1.f), matrix::wrap_pi(i * -.2f), 1e-5f);
	}
}

TEST(SlewRateYawTest, SlewUpWrappedInput)
{
	// put the goal value always a bit further away such that at some point the output has to wrap
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(.2f);
	_slew_rate_yaw.setForcedValue(0.f);

	for (int i = 1; i <= 50; i++) {
		EXPECT_NEAR(_slew_rate_yaw.update(matrix::wrap_pi(i * .25f), 1.f), matrix::wrap_pi(i * .2f), 1e-5f);
	}
}

TEST(SlewRateYawTest, SlewShortWayInput)
{
	SlewRateYaw<float> _slew_rate_yaw;
	_slew_rate_yaw.setSlewRate(1.f);
	_slew_rate_yaw.setForcedValue(0.f);

	EXPECT_FLOAT_EQ(_slew_rate_yaw.update(3.f, 1.f), 1.f);
	EXPECT_FLOAT_EQ(_slew_rate_yaw.update(5.f, 1.f), 0.f);
	EXPECT_FLOAT_EQ(_slew_rate_yaw.update(5.f, 1.f), -1.f);
	EXPECT_FLOAT_EQ(_slew_rate_yaw.update(4.f, 1.f), -2.f);
	EXPECT_FLOAT_EQ(_slew_rate_yaw.update(3.f, 1.f), -3.f);
	EXPECT_FLOAT_EQ(_slew_rate_yaw.update(5.f, 1.f), -2.f);
}
