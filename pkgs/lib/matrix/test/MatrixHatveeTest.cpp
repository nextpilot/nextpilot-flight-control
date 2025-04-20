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
#include <matrix/math.hpp>

using namespace matrix;

TEST(MatrixHatveeTest, Hatvee)
{
	Euler<float> euler(0.1f, 0.2f, 0.3f);
	Dcm<float> R(euler);
	Dcm<float> skew = R - R.T();
	Vector3<float> w = skew.vee();
	Vector3<float> w_check(0.1348f, 0.4170f, 0.5647f);

	EXPECT_EQ(w, w_check);
	EXPECT_EQ(skew, w.hat());
}
