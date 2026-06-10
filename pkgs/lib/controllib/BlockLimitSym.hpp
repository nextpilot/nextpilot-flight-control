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
 * A symmetric limiter/ saturation.
 * Same as limiter but with only a max, is used for
 * upper limit of +max, and lower limit of -max
 */
class __EXPORT BlockLimitSym : public Block
{
public:
// methods
	BlockLimitSym(SuperBlock *parent, const char *name) :
		Block(parent, name),
		_max(this, "MAX")
	{}
	virtual ~BlockLimitSym() = default;
	float update(float input);
// accessors
	float getMax() { return _max.get(); }
protected:
// attributes
	control::BlockParamFloat _max;
};

} // namespace control
