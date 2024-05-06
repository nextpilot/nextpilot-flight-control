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

TEST(MatrixMultiplicationTest, Multiplication)
{
	float data[9] = {1, 0, 0, 0, 1, 0, 1, 0, 1};
	Matrix3f A(data);

	float data_check[9] = {1, 0, 0, 0, 1, 0, -1, 0, 1};
	Matrix3f A_I(data_check);
	Matrix3f I;
	I.setIdentity();
	Matrix3f R = A * A_I;
	EXPECT_EQ(R, I);

	Matrix3f R2 = A;
	R2 *= A_I;
	EXPECT_EQ(R2, I);

	EXPECT_EQ(R2, I);
	EXPECT_NE(A, A_I);
	Matrix3f A2 = eye<float, 3>() * 2;
	Matrix3f B = A2.emult(A2);
	Matrix3f B_check = eye<float, 3>() * 4;
	Matrix3f C_check = eye<float, 3>() * 2;
	EXPECT_EQ(B, B_check);
	Matrix3f C = B_check.edivide(C_check);

	float off_diagonal_nan[9] = {2, NAN, NAN, NAN, 2, NAN, NAN, NAN, 2};
	// off diagonal are NANs because division by 0
	EXPECT_EQ(C, Matrix3f(off_diagonal_nan));

	// Test non-square matrix
	float data_43[12] = {1, 3, 2,
			     2, 2, 1,
			     5, 2, 1,
			     2, 3, 4
			    };
	float data_32[6] = {2, 3,
			    1, 7,
			    5, 4
			   };

	Matrix<float, 4, 3> m43(data_43);
	Matrix<float, 3, 2> m32(data_32);

	Matrix<float, 4, 2> m42 = m43 * m32;

	float data_42[8] = {15, 32,
			    11, 24,
			    17, 33,
			    27, 43
			   };
	Matrix<float, 4, 2> m42_check(data_42);
	EXPECT_EQ(m42, m42_check);

	float data_42_plus2[8] = {17, 34,
				  13, 26,
				  19, 35,
				  29, 45
				 };
	Matrix<float, 4, 2> m42_plus2_check(data_42_plus2);
	Matrix<float, 4, 2> m42_plus2 = m42 - (-2);
	EXPECT_EQ(m42_plus2, m42_plus2_check);
}
