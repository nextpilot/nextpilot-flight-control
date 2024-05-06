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

TEST(MatrixSetIdentityTest, SetIdentity)
{
	Matrix3f A;
	A.setIdentity();

	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (i == j) {
				EXPECT_FLOAT_EQ(A(i, j), 1);

			} else {
				EXPECT_FLOAT_EQ(A(i, j), 0);
			}
		}
	}

	Matrix3f B;
	B.identity();

	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (i == j) {
				EXPECT_FLOAT_EQ(B(i, j), 1);

			} else {
				EXPECT_FLOAT_EQ(B(i, j), 0);
			}
		}
	}
}
