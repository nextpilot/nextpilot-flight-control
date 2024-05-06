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

TEST(MatrixUpperRightTriangleTest, UpperRightTriangle)
{
	float data[9] = {1, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	float urt[6] = {1, 2, 3, 5, 6, 10};

	SquareMatrix<float, 3> A(data);

	for (size_t i = 0; i < 6; i++) {
		EXPECT_FLOAT_EQ(urt[i], A.upper_right_triangle()(i));
	}
}
