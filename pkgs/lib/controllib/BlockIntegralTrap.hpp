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
 * A trapezoidal integrator.
 * http://en.wikipedia.org/wiki/Trapezoidal_rule
 * A limiter is built into the class to bound the
 * integral's internal state. This is important
 * for windup protection.
 * @see Limit
 */
class __EXPORT BlockIntegralTrap : public SuperBlock
{
public:
// methods
	BlockIntegralTrap(SuperBlock *parent, const char *name) :
		SuperBlock(parent, name),
		_u(0),
		_y(0),
		_limit(this, "") {}
	virtual ~BlockIntegralTrap() = default;
	float update(float input);
// accessors
	float getU() {return _u;}
	float getY() {return _y;}
	float getMax() {return _limit.getMax();}
	void setU(float u) {_u = u;}
	void setY(float y) {_y = y;}
protected:
// attributes
	float _u; /**< previous input */
	float _y; /**< previous output */
	BlockLimitSym _limit; /**< limiter */
};

} // namespace control
