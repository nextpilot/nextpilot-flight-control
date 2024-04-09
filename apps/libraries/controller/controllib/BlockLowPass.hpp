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
#include <float.h>
#include <mathlib/math/test/test.hpp>
#include <mathlib/math/filter/LowPassFilter2p.hpp>

#include "block/Block.hpp"
#include "block/BlockParam.hpp"

#include "matrix/math.hpp"

namespace control
{

/**
 * A low pass filter as described here:
 * http://en.wikipedia.org/wiki/Low-pass_filter.
 */
class __EXPORT BlockLowPass : public Block
{
public:
// methods
	BlockLowPass(SuperBlock *parent, const char *name) :
		Block(parent, name),
		_fCut(this, "") // only one parameter, no need to name
	{}
	virtual ~BlockLowPass() = default;
	float update(float input);
// accessors
	float getState() { return _state; }
	float getFCut() { return _fCut.get(); }
	void setState(float state) { _state = state; }
protected:
// attributes
	float _state{NAN}; // initialize to invalid val, force into is_finite() check on first call
	control::BlockParamFloat _fCut;
};

} // namespace control
