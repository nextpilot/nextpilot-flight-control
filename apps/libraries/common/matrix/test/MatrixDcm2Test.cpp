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
#include <matrix/Dcm.hpp>
#include <matrix/Dcm2.hpp>

using namespace matrix;

TEST(MatrixDcm2Test, CompareToDcm)
{
	// 2D matrix and 3D matrix rotating around yaw are equivalent
	Dcm2f R2(1.f);
	Dcmf R3(Eulerf(0.f, 0.f, 1.f));
	EXPECT_EQ(Dcm2f(R3.slice<2, 2>(0, 0)), R2);

	// Rotating a 2D and a 3D vector respectively is equivalent
	Vector2f v2(1.f, 2.f);
	Vector3f v3(v2(0), v2(1), 0.f);
	Vector2f result2 = R2 * v2;
	Vector3f result3 = R3 * v3;
	EXPECT_EQ(Vector2f(result2), result2);
	EXPECT_EQ(result2, result3.xy());
}

TEST(MatrixDcm2Test, 45DegreeRotation)
{
	Vector2f v(1.0f, 0.0f);
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(1.f, 1.f) * (sqrt(2) / 2));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(0.f, 1.f));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(-1.f, 1.f) * (sqrt(2) / 2));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(-1.f, 0.f));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(-1.f, -1.f) * (sqrt(2) / 2));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(0.f, -1.f));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(1.f, -1.f) * (sqrt(2) / 2));
	v = Dcm2f(M_PI / 4.f) * v;
	EXPECT_EQ(v, Vector2f(1.f, 0.f));
}
