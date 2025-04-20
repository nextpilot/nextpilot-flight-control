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
#include <matrix/integration.hpp>

using namespace matrix;

Vector<float, 6> f(float t, const Matrix<float, 6, 1> &  /*y*/, const Matrix<float, 3, 1> &  /*u*/)
{
	float v = -sin(t);
	return v * ones<float, 6, 1>();
}

TEST(MatrixIntegralTest, Integral)
{
	Vector<float, 6> y = ones<float, 6, 1>();
	Vector<float, 3> u = ones<float, 3, 1>();
	float t0 = 0;
	float tf = 2;
	float h = 0.001f;
	integrate_rk4(f, y, u, t0, tf, h, y);
	float v = 1 + std::cos(tf) - std::cos(t0);
	EXPECT_EQ(y, (ones<float, 6, 1>()*v));
}
