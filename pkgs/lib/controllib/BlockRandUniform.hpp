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
 * @file blocks.h
 *
 * Controller library code
 */

#pragma once

#include <defines.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <mathlib/math/test/test.hpp>
#include <mathlib/math/filter/LowPassFilter2p.hpp>

#include "block/Block.hpp"
#include "block/BlockParam.hpp"

#include "matrix/math.hpp"

namespace control
{

/**
 * A uniform random number generator
 */
class __EXPORT BlockRandUniform: public Block
{
public:
// methods
	BlockRandUniform(SuperBlock *parent,
			 const char *name) :
		Block(parent, name),
		_min(this, "MIN"),
		_max(this, "MAX")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	virtual ~BlockRandUniform() = default;
	float update()
	{
		static float rand_max = static_cast<float>(RAND_MAX);
		float rand_val = rand();
		float bounds = getMax() - getMin();
		return getMin() + (rand_val * bounds) / rand_max;
	}
// accessors
	float getMin() { return _min.get(); }
	float getMax() { return _max.get(); }
private:
// attributes
	control::BlockParamFloat _min;
	control::BlockParamFloat _max;
};

} // namespace control
