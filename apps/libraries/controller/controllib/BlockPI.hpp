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
 * A proportional-integral controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
class __EXPORT BlockPI: public SuperBlock
{
public:
// methods
	BlockPI(SuperBlock *parent, const char *name) :
		SuperBlock(parent, name),
		_integral(this, "I"),
		_kP(this, "P"),
		_kI(this, "I")
	{}
	virtual ~BlockPI() = default;
	float update(float input)
	{
		return getKP() * input +
		       getKI() * getIntegral().update(input);
	}
// accessors
	float getKP() { return _kP.get(); }
	float getKI() { return _kI.get(); }
	BlockIntegral &getIntegral() { return _integral; }
private:
	BlockIntegral _integral;
	control::BlockParamFloat _kP;
	control::BlockParamFloat _kI;
};

} // namespace control
