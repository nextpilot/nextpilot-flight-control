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
 * @file test_EKF_utils.cpp
 *
 * @brief Unit tests for the miscellaneous EKF utilities
 */

#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <mathlib/mathlib.h>

using namespace math::Utilities;

TEST(euler312YawTest, fromQuaternion)
{
	matrix::Quatf q1(3.5f, 2.4f, -0.5f, -3.f);
	q1.normalize();
	const matrix::Eulerf euler1(q1);
	EXPECT_FLOAT_EQ(euler1(2), getEuler321Yaw(q1));

	matrix::Quatf q2(0.f, 0, -1.f, 0.f);
	q2.normalize();
	const matrix::Eulerf euler2(q2);
	EXPECT_FLOAT_EQ(euler2(2), getEuler321Yaw(q2));
}

TEST(shouldUse321RotationSequenceTest, pitch90)
{
	matrix::Eulerf euler(0.f, math::radians(90), 0.f);
	matrix::Dcmf R(euler);
	EXPECT_FALSE(shouldUse321RotationSequence(R));
}

TEST(shouldUse321RotationSequenceTest, roll90)
{
	matrix::Eulerf euler(math::radians(90.f), 0.f, 0.f);
	matrix::Dcmf R(euler);
	EXPECT_TRUE(shouldUse321RotationSequence(R));
}

TEST(shouldUse321RotationSequenceTest, moreRollThanPitch)
{
	matrix::Eulerf euler(math::radians(45.f), math::radians(30.f), 0.f);
	matrix::Dcmf R(euler);
	EXPECT_TRUE(shouldUse321RotationSequence(R));
}

TEST(shouldUse321RotationSequenceTest, morePitchThanRoll)
{
	matrix::Eulerf euler(math::radians(30.f), math::radians(45.f), 0.f);
	matrix::Dcmf R(euler);
	EXPECT_FALSE(shouldUse321RotationSequence(R));
}
