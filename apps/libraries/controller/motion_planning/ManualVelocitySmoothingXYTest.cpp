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
 * Test code for the Manual Velocity Smoothing library
 * Run this test only using make tests TESTFILTER=ManualVelocitySmoothing
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>

#include <motion_planning/ManualVelocitySmoothingXY.hpp>

using namespace matrix;

class ManualVelocitySmoothingXYTest : public ::testing::Test
{
public:
	ManualVelocitySmoothingXY _smoothing;
};


TEST_F(ManualVelocitySmoothingXYTest, setGet)
{
	// GIVEN: Some max values
	_smoothing.setMaxJerk(11.f);
	_smoothing.setMaxAccel(7.f);
	_smoothing.setMaxVel(5.f);

	// THEN: We should be able to get them back
	EXPECT_FLOAT_EQ(_smoothing.getMaxJerk(), 11.f);
	EXPECT_FLOAT_EQ(_smoothing.getMaxAccel(), 7.f);
	EXPECT_FLOAT_EQ(_smoothing.getMaxVel(), 5.f);
}

TEST_F(ManualVelocitySmoothingXYTest, getCurrentState)
{
	// GIVEN: the initial conditions
	Vector2f v0(11.f, 13.f);
	_smoothing.setCurrentVelocity(v0);

	// WHEN: we get the current state
	Vector3f j_end;
	Vector3f a_end;
	Vector3f v_end;
	Vector3f x_end;
	j_end.xy() = _smoothing.getCurrentJerk();
	a_end.xy() = _smoothing.getCurrentAcceleration();
	v_end.xy() = _smoothing.getCurrentVelocity();
	x_end.xy() = _smoothing.getCurrentPosition();

	// THEN: the returned values should match the input
	EXPECT_EQ(j_end, Vector3f(0.f, 0.f, 0.f));
	EXPECT_EQ(a_end, Vector3f(0.f, 0.f, 0.f));
	EXPECT_EQ(v_end, Vector3f(11.f, 13.f, 0.f));
	EXPECT_EQ(x_end, Vector3f(0.f, 0.f, 0.f));
}
