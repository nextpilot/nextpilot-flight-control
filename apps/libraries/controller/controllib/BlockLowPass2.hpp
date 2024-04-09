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
 * A 2nd order low pass filter block which uses the default px4 2nd order low pass filter
 */
class __EXPORT BlockLowPass2 : public Block
{
public:
// methods
	BlockLowPass2(SuperBlock *parent, const char *name, float sample_freq) :
		Block(parent, name),
		_state(0.0 / 0.0 /* initialize to invalid val, force into is_finite() check on first call */),
		_fCut(this, ""), // only one parameter, no need to name
		_fs(sample_freq),
		_lp(_fs, _fCut.get())
	{}
	virtual ~BlockLowPass2() = default;
	float update(float input);
// accessors
	float getState() { return _state; }
	float getFCutParam() { return _fCut.get(); }
	void setState(float state) { _state = _lp.reset(state); }
protected:
// attributes
	float _state;
	control::BlockParamFloat _fCut;
	float _fs;
	math::LowPassFilter2p<float> _lp;
};

} // namespace control
