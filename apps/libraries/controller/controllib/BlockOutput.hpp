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

#include <px4_platform_common/defines.h>
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
 * An output trim/ saturation block
 */
class __EXPORT BlockOutput: public SuperBlock
{
public:
// methods
	BlockOutput(SuperBlock *parent, const char *name) :
		SuperBlock(parent, name),
		_trim(this, "TRIM"),
		_limit(this, ""),
		_val(0)
	{
		update(0);
	}

	virtual ~BlockOutput() = default;
	void update(float input)
	{
		_val = _limit.update(input + getTrim());
	}
// accessors
	float getMin() { return _limit.getMin(); }
	float getMax() { return _limit.getMax(); }
	float getTrim() { return _trim.get(); }
	float get() { return _val; }
private:
// attributes
	control::BlockParamFloat _trim;
	BlockLimit _limit;
	float _val;
};

} // namespace control
