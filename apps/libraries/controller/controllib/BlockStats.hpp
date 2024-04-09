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
class __EXPORT BlockStats: public Block
{

public:
// methods
	BlockStats(SuperBlock *parent,
		   const char *name) :
		Block(parent, name),
		_sum(),
		_sumSq(),
		_count(0)
	{}
	virtual ~BlockStats() = default;
	void update(const matrix::Vector<Type, M> &u)
	{
		_sum += u;
		_sumSq += u.emult(u);
		_count += 1;
	}
	void reset()
	{
		_sum.setZero();
		_sumSq.setZero();
		_count = 0;
	}
// accessors
	size_t getCount() { return _count; }
	matrix::Vector<Type, M> getMean() { return _sum / _count; }
	matrix::Vector<Type, M> getVar()
	{
		return (_sumSq - _sum.emult(_sum) / _count) / _count;
	}
	matrix::Vector<Type, M> getStdDev()
	{
		return getVar().sqrt();
	}
private:
// attributes
	matrix::Vector<Type, M> _sum;
	matrix::Vector<Type, M> _sumSq;
	size_t _count;
};

} // namespace control
