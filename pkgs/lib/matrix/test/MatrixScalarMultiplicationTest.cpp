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

TEST(MatrixScalarMultiplicationTest, ScalarMultiplication)
{
	float data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	Matrix3f A(data);
	A = A * 2;
	float data_check[9] = {2, 4, 6, 8, 10, 12, 14, 16, 18};
	Matrix3f A_check(data_check);
	EXPECT_EQ(A, A_check);
}
