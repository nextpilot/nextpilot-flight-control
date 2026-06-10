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
 * @file hysteresis_test.cpp
 * Tests for system timing hysteresis.
 */

#include <gtest/gtest.h>

#include "hysteresis.h"


static constexpr hrt_abstime SOME_START_TIME = 1558359616134000llu;


TEST(Hysteresis, InitFalse)
{
	nextpilot::Hysteresis hysteresis(false);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, InitTrue)
{
	nextpilot::Hysteresis hysteresis(true);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Zero)
{
	hrt_abstime time_us = SOME_START_TIME;

	// Default is 0 hysteresis.
	nextpilot::Hysteresis hysteresis(false);
	EXPECT_FALSE(hysteresis.get_state());

	// Change and see result immediately.
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_TRUE(hysteresis.get_state());
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_TRUE(hysteresis.get_state());

	time_us += 1000;
	// A wait won't change anything.
	hysteresis.update(time_us);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, ChangeAfterTime)
{
	hrt_abstime time_us = SOME_START_TIME;

	nextpilot::Hysteresis hysteresis(false);
	hysteresis.set_hysteresis_time_from(false, 5000);
	hysteresis.set_hysteresis_time_from(true, 3000);

	// Change to true.
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 4000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 2000;
	hysteresis.update(time_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change back to false.
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_TRUE(hysteresis.get_state());
	time_us += 1000;
	hysteresis.update(time_us);
	EXPECT_TRUE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, HysteresisChanged)
{
	hrt_abstime time_us = SOME_START_TIME;

	nextpilot::Hysteresis hysteresis(false);
	hysteresis.set_hysteresis_time_from(true, 2000);
	hysteresis.set_hysteresis_time_from(false, 5000);

	// Change to true.
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.update(time_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change hysteresis time.
	hysteresis.set_hysteresis_time_from(true, 10000);

	// Change back to false.
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_TRUE(hysteresis.get_state());
	time_us += 7000;
	hysteresis.update(time_us);
	EXPECT_TRUE(hysteresis.get_state());
	time_us += 5000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, ChangeAfterMultipleSets)
{
	hrt_abstime time_us = SOME_START_TIME;

	nextpilot::Hysteresis hysteresis(false);
	hysteresis.set_hysteresis_time_from(true, 5000);
	hysteresis.set_hysteresis_time_from(false, 5000);

	// Change to true.
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change to false.
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_TRUE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_TRUE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, TakeChangeBack)
{
	hrt_abstime time_us = SOME_START_TIME;

	nextpilot::Hysteresis hysteresis(false);
	hysteresis.set_hysteresis_time_from(false, 5000);

	// Change to true.
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());
	// Change your mind to false.
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 6000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());

	// And true again
	hysteresis.set_state_and_update(true, time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.update(time_us);
	EXPECT_FALSE(hysteresis.get_state());
	time_us += 3000;
	hysteresis.update(time_us);
	EXPECT_TRUE(hysteresis.get_state());

	// The other directory is immediate.
	hysteresis.set_state_and_update(false, time_us);
	EXPECT_FALSE(hysteresis.get_state());
}
