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
 * @file test.cpp
 *
 * Test library code
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <px4_platform_common/defines.h>

#include "test.hpp"

bool __EXPORT equal(float a, float b, float epsilon)
{
	float diff = fabsf(a - b);

	if (diff > epsilon) {
		printf("not equal ->\n\ta: %12.8f\n\tb: %12.8f\n", double(a), double(b));
		return false;

	} else { return true; }
}

bool __EXPORT greater_than(float a, float b)
{
	if (a > b) {
		return true;

	} else {
		printf("not a > b ->\n\ta: %12.8f\n\tb: %12.8f\n", double(a), double(b));
		return false;
	}
}

bool __EXPORT less_than(float a, float b)
{
	if (a < b) {
		return true;

	} else {
		printf("not a < b ->\n\ta: %12.8f\n\tb: %12.8f\n", double(a), double(b));
		return false;
	}
}

bool __EXPORT greater_than_or_equal(float a, float b)
{
	if (a >= b) {
		return true;

	} else {
		printf("not a >= b ->\n\ta: %12.8f\n\tb: %12.8f\n", double(a), double(b));
		return false;
	}
}

bool __EXPORT less_than_or_equal(float a, float b)
{
	if (a <= b) {
		return true;

	} else {
		printf("not a <= b ->\n\ta: %12.8f\n\tb: %12.8f\n", double(a), double(b));
		return false;
	}
}

void __EXPORT float2SigExp(const float &num, float &sig, int &exp)
{

	if (!PX4_ISFINITE(num)) {
		sig = 0.0f;
		exp = -99;
		return;
	}

	if (fabsf(num) < 1.0e-38f) {
		sig = 0;
		exp = 0;
		return;
	}

	exp = log10f(fabsf(num));

	if (exp > 0) {
		exp = ceil(exp);

	} else {
		exp = floor(exp);
	}

	sig = num;

	// cheap power since it is integer
	if (exp > 0) {
		for (int i = 0; i < abs(exp); i++) { sig /= 10; }

	} else {
		for (int i = 0; i < abs(exp); i++) { sig *= 10; }
	}
}
