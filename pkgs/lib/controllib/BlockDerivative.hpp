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

#include "BlockLowPass.hpp"

#include <defines.h>
#include <math.h>

#include "block/Block.hpp"
#include "block/BlockParam.hpp"

#include "matrix/math.hpp"

namespace control
{

/**
 * A simple derivative approximation.
 * This uses the previous and current input.
 * This has a built in low pass filter.
 * @see LowPass
 */
class __EXPORT BlockDerivative : public SuperBlock
{
public:
// methods
	BlockDerivative(SuperBlock *parent, const char *name) :
		SuperBlock(parent, name),
		_u(0),
		_initialized(false),
		_lowPass(this, "LP")
	{}
	virtual ~BlockDerivative() = default;

	/**
	 * Update the state and get current derivative
	 *
	 * This call updates the state and gets the current
	 * derivative. As the derivative is only valid
	 * on the second call to update, it will return
	 * no change (0) on the first. To get a closer
	 * estimate of the derivative on the first call,
	 * call setU() one time step before using the
	 * return value of update().
	 *
	 * @param input the variable to calculate the derivative of
	 * @return the current derivative
	 */
	float update(float input);
// accessors
	void setU(float u) {_u = u;}
	void reset() { _initialized = false; };
	float getU() {return _u;}
	float getLP() {return _lowPass.getFCut();}
	float getO() { return _lowPass.getState(); }
protected:
// attributes
	float _u; /**< previous input */
	bool _initialized;
	BlockLowPass _lowPass; /**< low pass filter */
};

} // namespace control
