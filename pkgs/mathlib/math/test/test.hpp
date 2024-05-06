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
 * @file test.hpp
 *
 * Controller library code
 */

#pragma once

//#include <assert.h>
//#include <time.h>
//#include <stdlib.h>

bool equal(float a, float b, float eps = 1e-5);

bool greater_than(float a, float b);

bool less_than(float a, float b);

bool greater_than_or_equal(float a, float b);

bool less_than_or_equal(float a, float b);

void float2SigExp(
	const float &num,
	float &sig,
	int &exp);
