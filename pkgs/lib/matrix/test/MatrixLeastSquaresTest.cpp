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

int test_4x3(void);
template<typename Type> int test_4x4(void);
int test_4x4_type_double(void);
int test_div_zero(void);

TEST(MatrixLeastSquaresTest, 4x3)
{
	// Start with an (m x n) A matrix
	float data[12] = {20.f, -10.f, -13.f,
			  17.f,  16.f, -18.f,
			  0.7f,  -0.8f,   0.9f,
			  -1.f,  -1.1f,  -1.2f
			 };
	Matrix<float, 4, 3> A(data);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector3f x_check(-0.69168233f, -0.26227593f, -1.03767522f);

	LeastSquaresSolver<float, 4, 3> qrd = LeastSquaresSolver<float, 4, 3>(A);

	Vector3f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(MatrixLeastSquaresTest, 4x4)
{
	// Start with an (m x n) A matrix
	const float data[16] = { 20.f, -10.f, -13.f,  21.f,
				 17.f,  16.f, -18.f, -14.f,
				 0.7f,  -0.8f,   0.9f,  -0.5f,
				 -1.f,  -1.1f,  -1.2f,  -1.3f
			       };
	Matrix<float, 4, 4> A(data);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4f x_check(0.97893433f, -2.80798701f, -0.03175765f, -2.19387649f);

	LeastSquaresSolver<float, 4, 4> qrd = LeastSquaresSolver<float, 4, 4>(A);

	Vector4f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(MatrixLeastSquaresTest, ZeroDivision)
{
	float data[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	Matrix<float, 2, 2> A(data);

	float b_data[2] = {1.0f, 1.0f};
	Vector<float, 2> b(b_data);

	// Implement such that x returns zeros if it reaches div by zero
	float x_check_data[2] = {0.0f, 0.0f};
	Vector<float, 2> x_check(x_check_data);

	LeastSquaresSolver<float, 2, 2> qrd = LeastSquaresSolver<float, 2, 2>(A);

	Vector<float, 2> x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}
