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

TEST(MatrixTransposeTest, Transpose)
{
	float data[6] = {1, 2, 3, 4, 5, 6};
	Matrix<float, 2, 3> A(data);
	Matrix<float, 3, 2> A_T = A.transpose();
	float data_check[6] = {1, 4, 2, 5, 3, 6};
	Matrix<float, 3, 2> A_T_check(data_check);
	EXPECT_EQ(A_T, A_T_check);
}
