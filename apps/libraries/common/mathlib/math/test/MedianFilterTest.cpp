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
 * Test code for the Median filter
 * Run this test only using make tests TESTFILTER=MedianFilter
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>
#include <mathlib/mathlib.h>

#include <mathlib/math/filter/MedianFilter.hpp>

using namespace math;
using matrix::Vector3f;

class MedianFilterTest : public ::testing::Test
{
public:


};

TEST_F(MedianFilterTest, test3f_simple)
{
	MedianFilter<float, 3> median_filter3;

	for (int i = 0; i < 3; i++) {
		median_filter3.insert(i);
	}

	EXPECT_EQ(median_filter3.median(), 1); // 0, 1, 2
}

TEST_F(MedianFilterTest, test3f_100)
{
	MedianFilter<float, 3> median_filter3;

	for (int i = 0; i < 100; i++) {
		EXPECT_EQ(median_filter3.apply(i), max(0, i - 1));
	}
}

TEST_F(MedianFilterTest, test5u_simple)
{
	MedianFilter<uint16_t, 5> median_filter5;

	for (int i = 0; i < 5; i++) {
		median_filter5.insert(i);
	}

	EXPECT_EQ(median_filter5.median(), 2); // 0, 1, 2, 4, 5
}

TEST_F(MedianFilterTest, test5i_100)
{
	MedianFilter<uint16_t, 5> median_filter5;

	for (int i = 0; i < 100; i++) {
		EXPECT_EQ(median_filter5.apply(i), max(0, i - 2));
	}
}
