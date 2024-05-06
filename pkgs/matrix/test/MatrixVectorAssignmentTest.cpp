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

TEST(MatrixVectorAssignmentTest, VectorAssignment)
{
	Vector3f v;
	v(0) = 1;
	v(1) = 2;
	v(2) = 3;

	EXPECT_FLOAT_EQ(v(0), 1);
	EXPECT_FLOAT_EQ(v(1), 2);
	EXPECT_FLOAT_EQ(v(2), 3);

	Vector3f v2(4, 5, 6);

	EXPECT_FLOAT_EQ(v2(0), 4);
	EXPECT_FLOAT_EQ(v2(1), 5);
	EXPECT_FLOAT_EQ(v2(2), 6);

	SquareMatrix<float, 3> m = diag(Vector3f(1, 2, 3));
	EXPECT_FLOAT_EQ(m(0, 0), 1);
	EXPECT_FLOAT_EQ(m(1, 1), 2);
	EXPECT_FLOAT_EQ(m(2, 2), 3);
}
