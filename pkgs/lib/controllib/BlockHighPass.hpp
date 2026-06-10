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
 * A high pass filter as described here:
 * http://en.wikipedia.org/wiki/High-pass_filter.
 */
class __EXPORT BlockHighPass : public Block
{
public:
// methods
	BlockHighPass(SuperBlock *parent, const char *name) :
		Block(parent, name),
		_u(0),
		_y(0),
		_fCut(this, "") // only one parameter, no need to name
	{}
	virtual ~BlockHighPass() = default;
	float update(float input);
// accessors
	float getU() {return _u;}
	float getY() {return _y;}
	float getFCut() {return _fCut.get();}
	void setU(float u) {_u = u;}
	void setY(float y) {_y = y;}
protected:
// attributes
	float _u; /**< previous input */
	float _y; /**< previous output */
	control::BlockParamFloat _fCut; /**< cut-off frequency, Hz */
};

} // namespace control
