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

#include "BlockLimitSym.hpp"

#include "block/Block.hpp"
#include "block/BlockParam.hpp"

#include "matrix/math.hpp"

namespace control
{

/**
 * A rectangular integrator.
 * A limiter is built into the class to bound the
 * integral's internal state. This is important
 * for windup protection.
 * @see Limit
 */
class __EXPORT BlockIntegral: public SuperBlock
{
public:
// methods
	BlockIntegral(SuperBlock *parent, const char *name) :
		SuperBlock(parent, name),
		_y(0),
		_limit(this, "") {}
	virtual ~BlockIntegral() = default;
	float update(float input);
// accessors
	float getY() {return _y;}
	float getMax() {return _limit.getMax();}
	void setY(float y) {_y = y;}
protected:
// attributes
	float _y; /**< previous output */
	BlockLimitSym _limit; /**< limiter */
};

} // namespace control
