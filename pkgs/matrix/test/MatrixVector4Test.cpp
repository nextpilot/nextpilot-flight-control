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

TEST(MatrixVector4Test, Vector4)
{
	Vector4f a(1.f, 2.f, 3.f, 4.f);
	EXPECT_EQ(a(0), 1.f);
	EXPECT_EQ(a(1), 2.f);
	EXPECT_EQ(a(2), 3.f);
	EXPECT_EQ(a(3), 4.f);
}
