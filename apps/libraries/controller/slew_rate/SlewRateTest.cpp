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
#include "SlewRate.hpp"

TEST(SlewRateTest, SlewUpLimited)
{
	SlewRate<float> _slew_rate;
	_slew_rate.setSlewRate(.1f);
	_slew_rate.setForcedValue(-5.5f);

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate.update(20.f, .2f), -5.5f + i * .02f);
	}
}

TEST(SlewRateTest, SlewDownLimited)
{
	SlewRate<float> _slew_rate;
	_slew_rate.setSlewRate(1.1f);
	_slew_rate.setForcedValue(17.3f);

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate.update(-50.f, .3f), 17.3f - i * .33f);
	}
}

TEST(SlewRateTest, ReachValueSlewed)
{
	SlewRate<float> _slew_rate;
	_slew_rate.setSlewRate(.2f);
	_slew_rate.setForcedValue(8.f);

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate.update(10.f, 1.f), 8.f + i * .2f);
	}

	for (int i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(_slew_rate.update(10.f, 1.f), 10.f);
	}
}
