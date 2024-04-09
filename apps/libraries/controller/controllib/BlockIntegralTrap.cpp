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

float BlockIntegralTrap::update(float input)
{
	// trapezoidal integration
	setY(_limit.update(getY() +
			   (getU() + input) / 2.0f * getDt()));
	setU(input);
	return getY();
}

} // namespace control
