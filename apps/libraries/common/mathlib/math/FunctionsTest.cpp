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
#include "Functions.hpp"

using namespace math;

TEST(FunctionsTest, signNoZero)
{
	EXPECT_FLOAT_EQ(signNoZero(-123.456f), -1.f);
	EXPECT_FLOAT_EQ(signNoZero(-1.f), -1.f);
	EXPECT_FLOAT_EQ(signNoZero(-.0001f), -1.f);
	EXPECT_FLOAT_EQ(signNoZero(0.f), 1.f);
	EXPECT_FLOAT_EQ(signNoZero(.0001f), 1.f);
	EXPECT_FLOAT_EQ(signNoZero(1.f), 1.f);
	EXPECT_FLOAT_EQ(signNoZero(123.456f), 1.f);
}

TEST(FunctionsTest, signFromBool)
{
	EXPECT_EQ(signFromBool(true), 1);
	EXPECT_EQ(signFromBool(false), -1);
	EXPECT_EQ(signFromBool(100), 1);
	EXPECT_EQ(signFromBool(-100), 1);
	EXPECT_EQ(signFromBool(0), -1);
}

TEST(FunctionsTest, expo)
{
	// input value limits
	EXPECT_FLOAT_EQ(expo(-12.f, .5f), -1.f);
	EXPECT_FLOAT_EQ(expo(-1.f, .5f), -1.f);
	EXPECT_FLOAT_EQ(expo(0.f, .5f), 0.f);
	EXPECT_FLOAT_EQ(expo(1.f, .5f), 1.f);
	EXPECT_FLOAT_EQ(expo(45.f, .5f), 1.f);

	// expo parameter limits
	EXPECT_FLOAT_EQ(expo(.1f, 0.f), .1f);
	EXPECT_FLOAT_EQ(expo(.1f, -12.f), .1f);
	EXPECT_FLOAT_EQ(expo(.1f, 1.f), .001f);
	EXPECT_FLOAT_EQ(expo(.1f, 12.f), .001f);

	// different in range values
	EXPECT_FLOAT_EQ(expo(.1f, .5f), .0505f);
	EXPECT_FLOAT_EQ(expo(-.2f, .5f), -.104f);
	EXPECT_FLOAT_EQ(expo(.3f, .7f), .1089f);
	EXPECT_FLOAT_EQ(expo(-.4f, .9f), -.0976f);
}

TEST(FunctionsTest, superexpo)
{
	// input value limits
	EXPECT_FLOAT_EQ(superexpo(-12.f, .5f, .5f), -1.f);
	EXPECT_FLOAT_EQ(superexpo(-1.f, .5f, .5f), -1.f);
	EXPECT_FLOAT_EQ(superexpo(0.f, .5f, .5f), 0.f);
	EXPECT_FLOAT_EQ(superexpo(1.f, .5f, .5f), 1.f);
	EXPECT_FLOAT_EQ(superexpo(45.f, .5f, .5f), 1.f);

	// superexpo parameter limits
	EXPECT_FLOAT_EQ(superexpo(.1f, 0.f, 0.f), .1f);
	EXPECT_FLOAT_EQ(superexpo(.1f, 0.f, -12.f), .1f);
	EXPECT_NEAR(superexpo(.1f, 0.f, 0.99f), .0011098779134295227f, 1e-8f);
	EXPECT_NEAR(superexpo(.1f, 0.f, 12.f), .0011098779134295227f, 1e-8f);

	// different in range values
	EXPECT_FLOAT_EQ(superexpo(.5f, 0.f, .5f), .33333333333f);
	EXPECT_FLOAT_EQ(superexpo(-.2f, .5f, .5f), -.057777781f);
	EXPECT_FLOAT_EQ(superexpo(.3f, .7f, .5f), .064058825f);
	EXPECT_FLOAT_EQ(superexpo(-.4f, .9f, .5f), -.061000008f);
}

TEST(FunctionsTest, deadzone)
{
	// input value limits
	EXPECT_FLOAT_EQ(deadzone(-12.f, .5f), -1.f);
	EXPECT_FLOAT_EQ(deadzone(-1.f, .5f), -1.f);
	EXPECT_FLOAT_EQ(deadzone(0.f, .5f), 0.f);
	EXPECT_FLOAT_EQ(deadzone(1.f, .5f), 1.f);
	EXPECT_FLOAT_EQ(deadzone(45.f, .5f), 1.f);

	// deadzone parameter limits
	EXPECT_FLOAT_EQ(deadzone(.1f, 0.f), .1f);
	EXPECT_FLOAT_EQ(deadzone(.1f, -12.f), .1f);
	EXPECT_FLOAT_EQ(deadzone(.1f, 1.f), 0.f);
	EXPECT_FLOAT_EQ(deadzone(.1f, 12.f), 0.f);

	// different in range values
	EXPECT_FLOAT_EQ(deadzone(.1f, 0.f), .1f);
	EXPECT_FLOAT_EQ(deadzone(.1f, .1f), 0.f);
	EXPECT_FLOAT_EQ(deadzone(.2f, .1f), 0.1111111111f);
	EXPECT_FLOAT_EQ(deadzone(-.2f, .5f), 0.f);
	EXPECT_FLOAT_EQ(deadzone(.7f, .3f), .57142854f);
	EXPECT_FLOAT_EQ(deadzone(-.9f, .4f), -.83333325f);
}

TEST(FunctionsTest, expo_deadzone)
{
	// input value limits
	EXPECT_FLOAT_EQ(expo_deadzone(-12.f, .5f, .5f), -1.f);
	EXPECT_FLOAT_EQ(expo_deadzone(-1.f, .5f, .5f), -1.f);
	EXPECT_FLOAT_EQ(expo_deadzone(0.f, .5f, .5f), 0.f);
	EXPECT_FLOAT_EQ(expo_deadzone(1.f, .5f, .5f), 1.f);
	EXPECT_FLOAT_EQ(expo_deadzone(45.f, .5f, .5f), 1.f);

	// different in range values
	EXPECT_FLOAT_EQ(expo_deadzone(.5f, 0.f, .5f), 0.f);
	EXPECT_FLOAT_EQ(expo_deadzone(-.5f, .5f, .2f), -.21386719f);
	EXPECT_FLOAT_EQ(expo_deadzone(.5f, .7f, .3f), .10204081f);
	EXPECT_FLOAT_EQ(expo_deadzone(-.5f, .9f, .4f), -.020833336f);
}

TEST(FunctionsTest, interpolate)
{
	// factor of *2, offset +1
	EXPECT_FLOAT_EQ(interpolate(-12.f, 0.f, 1.f, 1.f, 3.f), 1.f);
	EXPECT_FLOAT_EQ(interpolate(0.f, 0.f, 1.f, 1.f, 3.f), 1.f);
	EXPECT_FLOAT_EQ(interpolate(.25f, 0.f, 1.f, 1.f, 3.f), 1.5f);
	EXPECT_FLOAT_EQ(interpolate(.5f, 0.f, 1.f, 1.f, 3.f), 2.f);
	EXPECT_FLOAT_EQ(interpolate(.75f, 0.f, 1.f, 1.f, 3.f), 2.5f);
	EXPECT_FLOAT_EQ(interpolate(1.f, 0.f, 1.f, 1.f, 3.f), 3.f);
	EXPECT_FLOAT_EQ(interpolate(12.f, 0.f, 1.f, 1.f, 3.f), 3.f);

	// factor of *2, offset +3
	EXPECT_FLOAT_EQ(interpolate(-12.f, 1.f, 2.f, 4.f, 6.f), 4.f);
	EXPECT_FLOAT_EQ(interpolate(1.f, 1.f, 2.f, 4.f, 6.f), 4.f);
	EXPECT_FLOAT_EQ(interpolate(1.25f, 1.f, 2.f, 4.f, 6.f), 4.5f);
	EXPECT_FLOAT_EQ(interpolate(1.5f, 1.f, 2.f, 4.f, 6.f), 5.f);
	EXPECT_FLOAT_EQ(interpolate(1.75f, 1.f, 2.f, 4.f, 6.f), 5.5f);
	EXPECT_FLOAT_EQ(interpolate(2.f, 1.f, 2.f, 4.f, 6.f), 6.f);
	EXPECT_FLOAT_EQ(interpolate(12.f, 1.f, 2.f, 4.f, 6.f), 6.f);

	// corner case when x_low == x_high == value
	EXPECT_FLOAT_EQ(interpolate(1.f, 1.f, 1.f, 4.f, 6.f), 4.f);

	// corner case when x_low > x_high
	EXPECT_FLOAT_EQ(interpolate(1.05f, 1.1f, 1.f, 4.f, 6.f), 4.f);

	// corner case when y_low == y_high == value
	EXPECT_FLOAT_EQ(interpolate(1.f, 1.f, 1.f, 4.f, 6.f), 4.f);

	// corner case when y_low > y_high
	EXPECT_FLOAT_EQ(interpolate(1.5f, 1.f, 2.f, 6.f, 4.f), 5.f);

	// corner case when x_low == x_high == y_low == y_high == 0.f
	EXPECT_FLOAT_EQ(interpolate(0.f, 0.f, 0.f, 0.f, 0.f), 0.f);
}

TEST(FunctionsTest, interpolateNXY)
{
	float x[3] = {0.f, .5f, 1.5f};
	float y[3] = {1.f, 2.f, 3.f};

	// factor of *2, offset +1
	EXPECT_FLOAT_EQ(interpolateNXY(-12.f, x, y), 1.f);
	EXPECT_FLOAT_EQ(interpolateNXY(0.f, x, y), 1.f);
	EXPECT_FLOAT_EQ(interpolateNXY(.25f, x, y), 1.5f);
	EXPECT_FLOAT_EQ(interpolateNXY(.5f, x, y), 2.f);
	EXPECT_FLOAT_EQ(interpolateNXY(.75f, x, y), 2.25f);
	EXPECT_FLOAT_EQ(interpolateNXY(1.f, x, y), 2.5f);
	EXPECT_FLOAT_EQ(interpolateNXY(1.25f, x, y), 2.75f);
	EXPECT_FLOAT_EQ(interpolateNXY(1.5f, x, y), 3.f);
	EXPECT_FLOAT_EQ(interpolateNXY(12.f, x, y), 3.f);
}

TEST(FunctionsTest, sqrt_linear)
{
	EXPECT_FLOAT_EQ(sqrt_linear(-12.f), 0.f);
	EXPECT_FLOAT_EQ(sqrt_linear(-2.f), 0.f);
	EXPECT_FLOAT_EQ(sqrt_linear(0.f), 0.f);
	EXPECT_FLOAT_EQ(sqrt_linear(.5f), 0.70710678f);
	EXPECT_FLOAT_EQ(sqrt_linear(1.f), 1.f);
	EXPECT_FLOAT_EQ(sqrt_linear(2.f), 2.f);
	EXPECT_FLOAT_EQ(sqrt_linear(120.f), 120.f);
}

TEST(FunctionsTest, lerp)
{
	EXPECT_FLOAT_EQ(lerp(0.f, 1.f, -.123f), -.123f);
	EXPECT_FLOAT_EQ(lerp(0.f, 1.f, 0.f), 0.f);
	EXPECT_FLOAT_EQ(lerp(0.f, 1.f, .123f), .123f);
	EXPECT_FLOAT_EQ(lerp(0.f, 1.f, 1.f), 1.f);
	EXPECT_FLOAT_EQ(lerp(0.f, 1.f, 1.123f), 1.123f);

	EXPECT_FLOAT_EQ(lerp(.2f, .3f, -.1f), .19f);
	EXPECT_FLOAT_EQ(lerp(-.4f, .3f, 1.1f), .37f);
}

TEST(FunctionsTest, countSetBits)
{
	EXPECT_EQ(countSetBits(255), 8);
	EXPECT_EQ(countSetBits(65535), 16);
	EXPECT_EQ(countSetBits(0), 0);
	EXPECT_EQ(countSetBits(0xffffffffu), 32);
	EXPECT_EQ(countSetBits(754323), 9);
}

TEST(FunctionsTest, isFiniteVector3f)
{
	EXPECT_TRUE(isFinite(matrix::Vector3f()));
	EXPECT_TRUE(isFinite(matrix::Vector3f(0.f, 1.f, 2.f)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(0.f, 0.f, INFINITY)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(0.f, 0.f, NAN)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(0.f, NAN, 0.f)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(0.f, NAN, NAN)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(NAN, 0.f, 0.f)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(NAN, 0.f, NAN)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(NAN, NAN, 0.f)));
	EXPECT_FALSE(isFinite(matrix::Vector3f(NAN, NAN, NAN)));
}
