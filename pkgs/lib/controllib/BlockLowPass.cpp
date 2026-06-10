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
 * @file blocks.cpp
 *
 * Controller library code
 */

#include <math.h>
#include <float.h>

#include "blocks.hpp"

namespace control
{

float BlockLowPass::update(float input)
{
	if (!PX4_ISFINITE(getState())) {
		setState(input);
	}

	float b = 2 * M_PI_F * getFCut() * getDt();
	float a = b / (1 + b);
	setState(a * input + (1 - a)*getState());
	return getState();
}

} // namespace control
