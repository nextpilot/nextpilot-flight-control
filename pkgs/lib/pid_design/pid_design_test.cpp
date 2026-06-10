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
 * Test code for pid_design functions
 * Run this test only using make tests TESTFILTER=pid_design
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>

#include "pid_design.hpp"

using namespace pid_design;
using namespace matrix;

TEST(PidDesignTest, testImpl)
{
	const Vector3f num(.0098f, -0.162f, .147f);
	const Vector3f den(1.f, -1.814f, .822f);
	const float dt = 0.004f;
	const Vector3f kid = computePidGmvc(num, den, dt, 0.1f, 1.f, 0.5f);

	// Compare with values generated from the python implementation
	EXPECT_NEAR(kid(0), 0.129f, .001f);
	EXPECT_NEAR(kid(1), 11.911f / 5.f, .001f);
	EXPECT_NEAR(kid(2), 0.0463f, .0001f);
}
