
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

TEST(MatrixVector2Test, Vector2)
{
	Vector2f a(1, 0);
	Vector2f b(0, 1);
	EXPECT_FLOAT_EQ(a % b, 1.f);

	Vector2f c;
	EXPECT_FLOAT_EQ(c(0), 0.f);
	EXPECT_FLOAT_EQ(c(1), 0.f);

	Matrix<float, 2, 1> d(a);
	EXPECT_FLOAT_EQ(d(0, 0), 1.f);
	EXPECT_FLOAT_EQ(d(1, 0), 0.f);

	Vector2f e(d);
	EXPECT_FLOAT_EQ(e(0), 1.f);
	EXPECT_FLOAT_EQ(e(1), 0.f);

	float data[] = {4, 5};
	Vector2f f(data);
	EXPECT_FLOAT_EQ(f(0), 4.f);
	EXPECT_FLOAT_EQ(f(1), 5.f);

	Vector3f g(1.23f, 423.4f, 3221.f);
	Vector2f h(g);
	EXPECT_FLOAT_EQ(h(0), 1.23f);
	EXPECT_FLOAT_EQ(h(1), 423.4f);
}
