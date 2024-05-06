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

TEST(MatrixVector3Test, Vector3)
{
	Vector3f a(1, 0, 0);
	Vector3f b(0, 1, 0);
	Vector3f c = a.cross(b);
	EXPECT_EQ(c, Vector3f(0, 0, 1));
	c = a % b;
	EXPECT_EQ(c, Vector3f(0, 0, 1));
	Matrix<float, 3, 1> d(c);
	Vector3f e(d);
	EXPECT_EQ(e, d);
	float data[] = {4, 5, 6};
	Vector3f f(data);
	EXPECT_EQ(f, Vector3f(4, 5, 6));

	EXPECT_EQ(a + b, Vector3f(1, 1, 0));
	EXPECT_EQ(a - b, Vector3f(1, -1, 0));
	EXPECT_FLOAT_EQ(a * b, 0.0f);
	EXPECT_EQ(-a, Vector3f(-1, 0, 0));
	EXPECT_EQ(a.unit(), a);
	EXPECT_EQ(a.unit(), a.normalized());
	EXPECT_EQ(a * 2.0, Vector3f(2, 0, 0));

	Vector2f g2(1, 3);
	Vector3f g3(7, 11, 17);
	g3.xy() = g2;
	EXPECT_EQ(g3, Vector3f(1, 3, 17));

	const Vector3f g4(g3);
	Vector2f g5 = g4.xy();
	EXPECT_EQ(g5, g2);
	EXPECT_EQ(g2, Vector2f(g4.xy()));

	Vector3f h;
	EXPECT_EQ(h, Vector3f(0, 0, 0));
	Vector4f j(1.f, 2.f, 3.f, 4.f);
	Vector3f k = j.slice<3, 1>(0, 0);
	Vector3f k_test(1, 2, 3);
	EXPECT_EQ(k, k_test);

	Vector3f m1(1, 2, 3);
	Vector3f m2(3.1f, 4.1f, 5.1f);
	EXPECT_EQ(m2, m1 + 2.1f);
	EXPECT_EQ(m2 - 2.1f, m1);
}
