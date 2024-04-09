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
 * A proportional controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
class __EXPORT BlockP: public Block
{
public:
// methods
	BlockP(SuperBlock *parent, const char *name) :
		Block(parent, name),
		_kP(this, "") // only one param, no need to name
	{}
	virtual ~BlockP() = default;
	float update(float input)
	{
		return getKP() * input;
	}
// accessors
	float getKP() { return _kP.get(); }
protected:
	control::BlockParamFloat _kP;
};

} // namespace control
