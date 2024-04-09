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

TEST(MatrixCopyToTest, CopyTo)
{
	// Vector3 copyTo
	const Vector3f v(1, 2, 3);
	float dst3[3] = {};
	v.copyTo(dst3);

	for (size_t i = 0; i < 3; i++) {
		EXPECT_FLOAT_EQ(v(i), dst3[i]);
	}

	// Quaternion copyTo
	Quatf q(1, 2, 3, 4);
	float dst4[4] = {};
	q.copyTo(dst4);

	for (size_t i = 0; i < 4; i++) {
		EXPECT_FLOAT_EQ(q(i), dst4[i]);
	}

	// Matrix copyTo
	Matrix<float, 2, 3> A;
	A(0, 0) = 1;
	A(0, 1) = 2;
	A(0, 2) = 3;
	A(1, 0) = 4;
	A(1, 1) = 5;
	A(1, 2) = 6;
	float array_A[6] = {};
	A.copyTo(array_A);
	float array_row[6] = {1, 2, 3, 4, 5, 6};

	for (size_t i = 0; i < 6; i++) {
		EXPECT_FLOAT_EQ(array_A[i], array_row[i]);
	}

	// Matrix copyToColumnMajor
	A.copyToColumnMajor(array_A);
	float array_column[6] = {1, 4, 2, 5, 3, 6};

	for (size_t i = 0; i < 6; i++) {
		EXPECT_FLOAT_EQ(array_A[i], array_column[i]);
	}

	// Slice copyTo
	float dst5[2] = {};
	v.slice<2, 1>(0, 0).copyTo(dst5);

	for (size_t i = 0; i < 2; i++) {
		EXPECT_FLOAT_EQ(v(i), dst5[i]);
	}

	float subarray_A[4] = {};
	A.slice<2, 2>(0, 0).copyToColumnMajor(subarray_A);
	float subarray_column[4] = {1, 4, 2, 5};

	for (size_t i = 0; i < 4; i++) {
		EXPECT_FLOAT_EQ(subarray_A[i], subarray_column[i]);
	}
}
