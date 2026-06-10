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
 * A proportional-integral-derivative controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
class __EXPORT BlockPID: public SuperBlock
{
public:
// methods
	BlockPID(SuperBlock *parent, const char *name) :
		SuperBlock(parent, name),
		_integral(this, "I"),
		_derivative(this, "D"),
		_kP(this, "P"),
		_kI(this, "I"),
		_kD(this, "D")
	{}
	virtual ~BlockPID() = default;
	float update(float input)
	{
		return getKP() * input +
		       getKI() * getIntegral().update(input) +
		       getKD() * getDerivative().update(input);
	}
// accessors
	float getKP() { return _kP.get(); }
	float getKI() { return _kI.get(); }
	float getKD() { return _kD.get(); }
	BlockIntegral &getIntegral() { return _integral; }
	BlockDerivative &getDerivative() { return _derivative; }
private:
// attributes
	BlockIntegral _integral;
	BlockDerivative _derivative;
	control::BlockParamFloat _kP;
	control::BlockParamFloat _kI;
	control::BlockParamFloat _kD;
};

} // namespace control
