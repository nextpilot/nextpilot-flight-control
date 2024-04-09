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

template<class Type, size_t M>
class __EXPORT BlockLowPassVector: public Block
{
public:
// methods
	BlockLowPassVector(SuperBlock *parent,
			   const char *name) :
		Block(parent, name),
		_state(),
		_fCut(this, "") // only one parameter, no need to name
	{
		for (size_t i = 0; i < M; i++) {
			_state(i) = 0.0f / 0.0f;
		}
	}
	virtual ~BlockLowPassVector() = default;
	matrix::Vector<Type, M> update(const matrix::Matrix<Type, M, 1> &input)
	{
		for (size_t i = 0; i < M; i++) {
			if (!PX4_ISFINITE(getState()(i))) {
				setState(input);
			}
		}

		float b = 2 * M_PI_F * getFCut() * getDt();
		float a = b / (1 + b);
		setState(input * a + getState() * (1 - a));
		return getState();
	}
// accessors
	matrix::Vector<Type, M> getState() { return _state; }
	float getFCut() { return _fCut.get(); }
	void setState(const matrix::Vector<Type, M> &state) { _state = state; }
private:
// attributes
	matrix::Vector<Type, M> _state;
	control::BlockParamFloat _fCut;
};

} // namespace control
