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
 * A limiter/ saturation.
 * The output of update is the input, bounded
 * by min/max.
 */
class __EXPORT BlockLimit : public Block
{
public:
// methods
	BlockLimit(SuperBlock *parent, const char *name) :
		Block(parent, name),
		_min(this, "MIN"),
		_max(this, "MAX")
	{}
	virtual ~BlockLimit() = default;
	float update(float input);
// accessors
	float getMin() { return _min.get(); }
	float getMax() { return _max.get(); }
protected:
// attributes
	control::BlockParamFloat _min;
	control::BlockParamFloat _max;
};

} // namespace control
