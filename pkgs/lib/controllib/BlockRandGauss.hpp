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

class __EXPORT BlockRandGauss: public Block
{
public:
// methods
	BlockRandGauss(SuperBlock *parent,
		       const char *name) :
		Block(parent, name),
		_mean(this, "MEAN"),
		_stdDev(this, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	virtual ~BlockRandGauss() = default;
	float update()
	{
		static float V1, V2, S;
		static int phase = 0;
		float X;

		if (phase == 0) {
			do {
				float U1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				float U2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * float(sqrtf(-2 * float(logf(S)) / S));

		} else {
			X = V2 * float(sqrtf(-2 * float(logf(S)) / S));
		}

		phase = 1 - phase;
		return X * getStdDev() + getMean();
	}
// accessors
	float getMean() { return _mean.get(); }
	float getStdDev() { return _stdDev.get(); }
private:
// attributes
	control::BlockParamFloat _mean;
	control::BlockParamFloat _stdDev;
};

} // namespace control
