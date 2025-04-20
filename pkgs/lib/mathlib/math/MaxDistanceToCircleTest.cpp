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
#include <matrix/matrix/math.hpp>
#include "Functions.hpp"

#include "TrajMath.hpp"

using namespace math;
using matrix::Vector2f;

TEST(MaxDistanceToCircle, noDirection)
{
	Vector2f pos;
	Vector2f circle_center;
	float radius = 3.f;
	Vector2f direction;
	float distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_TRUE(isnan(distance));
}

TEST(MaxDistanceToCircle, insideCircle)
{
	// North position, South direction
	Vector2f pos(1.f, 0.f);
	Vector2f circle_center;
	float radius = 3.f;
	Vector2f direction(-1.f, 0.f);
	float distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, 4.f);

	// North position, West direction (direction doesn't need to be unit length)
	direction = Vector2f(0.f, -3.42f);
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, sqrtf(radius * radius - 1.f));

	// SE position, directed towards the center
	pos = Vector2f(-2.f, 1.f);
	direction = Vector2f(2.f, -1.f);
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, pos.norm() + radius);
}

TEST(MaxDistanceToCircle, outsideCircle)
{
	// North position, South direction
	Vector2f pos(4.f, 0.f);
	Vector2f circle_center;
	float radius = 3.f;
	Vector2f direction(-1.f, 0.f);
	float distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, pos(0) + radius);

	// looking away from the circle
	direction = Vector2f(0.f, -3.42f);
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_TRUE(isnan(distance));

	// SE position, directed towards the center
	pos = Vector2f(-4.f, 2.f);
	direction = Vector2f(2.f, -1.f);
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, pos.norm() + radius);
}

TEST(MaxDistanceToCircle, onCircle)
{
	// South, looking North
	Vector2f pos(-4.f, 1.f);
	Vector2f circle_center(-1.f, 1.f);
	float radius = 3.f;
	Vector2f direction(1.f, 0.f);
	float distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, 2.f * radius);

	// looking tangent to the circle
	direction = Vector2f(0.f, -3.42f);
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, 0.f);

	// looking away from the circle
	direction = Vector2f(-10.f, -3.42f);
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, 0.f);

	// SE position, directed towards the center
	pos = Vector2f(-sqrtf(2.f) / 2.f, sqrtf(2.f) / 2.f) * radius + circle_center;
	direction = -pos;
	distance = trajectory::getMaxDistanceToCircle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(distance, 2.f * radius);
}
