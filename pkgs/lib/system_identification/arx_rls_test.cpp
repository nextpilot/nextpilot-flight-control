/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * Test code for the ArxRls class
 * Run this test only using make tests TESTFILTER=arx_rls
 */

#include <gtest/gtest.h>
#include <matrix/matrix/math.hpp>

#include "arx_rls.hpp"

using namespace matrix;

class ArxRlsTest : public ::testing::Test
{
public:
	ArxRlsTest() {};
};

TEST_F(ArxRlsTest, test211)
{
	ArxRls<2, 1, 1> _rls;

	for (int i = 0; i < (2 + 1 + 1); i++) {
		// Fill the buffers with zeros
		_rls.update(0.f, 0.f);
	}

	_rls.update(1, 2);
	_rls.update(3, 4);
	_rls.update(5, 6);
	const Vector4f coefficients = _rls.getCoefficients();
	const Vector4f coefficients_check(-1.79f, 0.97f, 0.42f, -0.48f); // generated from Python script
	float eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
}

TEST_F(ArxRlsTest, test221)
{
	ArxRls<2, 2, 1> _rls;

	for (int i = 0; i < (2 + 2 + 1); i++) {
		// Fill the buffers with zeros
		_rls.update(0.f, 0.f);
	}

	_rls.update(1, 2);
	_rls.update(3, 4);
	_rls.update(5, 6);
	_rls.update(7, 8);
	const Vector<float, 5> coefficients = _rls.getCoefficients();
	float data_check[] = {-1.81, 1.06f, 0.38f, -0.27f, 0.26f};
	const Vector<float, 5> coefficients_check(data_check);
	float eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	coefficients_check.print();
}

TEST_F(ArxRlsTest, resetTest)
{
	ArxRls<2, 2, 1> _rls;
	_rls.update(1, 2);
	_rls.update(3, 4);
	_rls.update(5, 6);
	_rls.update(7, 8);
	const Vector<float, 5> coefficients = _rls.getCoefficients();

	// WHEN: resetting
	_rls.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_rls.getVariances().min() > 5000.f);
	EXPECT_TRUE(_rls.getCoefficients().abs().max() < 1e-8f);

	// AND WHEN: running the same sequence of inputs-outputs
	_rls.update(1, 2);
	_rls.update(3, 4);
	_rls.update(5, 6);
	_rls.update(7, 8);

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _rls.getCoefficients()).abs().max() < 1e-8f);
}
