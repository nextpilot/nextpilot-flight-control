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

TEST(MatrixVectorTest, Vector)
{
	// test data
	float data1[] = {1, 2, 3, 4, 5};
	float data2[] = {6, 7, 8, 9, 10};
	Vector<float, 5> v1(data1);
	Vector<float, 5> v2(data2);

	// copy constructor
	Vector<float, 5> v3(v2);
	EXPECT_EQ(v2, v3);

	// norm, dot product
	EXPECT_FLOAT_EQ(v1.norm(), 7.416198487095663f);
	EXPECT_FLOAT_EQ(v1.norm_squared(), v1.norm() * v1.norm());
	EXPECT_FLOAT_EQ(v1.norm(), v1.length());
	EXPECT_FLOAT_EQ(v1.dot(v2), 130.0f);
	EXPECT_FLOAT_EQ(v1.dot(v2), v1 * v2);

	// unit, unit_zero, normalize
	EXPECT_FLOAT_EQ(v2.unit().norm(), 1.f);
	EXPECT_FLOAT_EQ(v2.unit_or_zero().norm(), 1.f);
	EXPECT_FLOAT_EQ((Vector<float, 5>().unit_or_zero().norm()), 0.f);
	v2.normalize();
	EXPECT_FLOAT_EQ(v2.norm(), 1.f);

	// sqrt
	float data1_sq[] = {1, 4, 9, 16, 25};
	Vector<float, 5> v4(data1_sq);
	EXPECT_EQ(v1, v4.sqrt());

	// longerThan
	Vector<float, 2> v5;
	v5(0) = 3;
	v5(1) = 4;
	EXPECT_TRUE(v5.longerThan(4.99f));
	EXPECT_FALSE(v5.longerThan(5.f));
}
