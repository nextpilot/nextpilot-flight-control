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

float BlockLowPass2::update(float input)
{
	if (!PX4_ISFINITE(getState())) {
		setState(input);
	}

	if (fabsf(_lp.get_cutoff_freq() - getFCutParam()) > FLT_EPSILON) {
		_lp.set_cutoff_frequency(_fs, getFCutParam());
	}

	_state = _lp.apply(input);
	return _state;
}

} // namespace control
