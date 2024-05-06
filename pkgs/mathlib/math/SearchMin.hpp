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
 * @file Seach.hpp
 *
 * - Binary Search (ToDo)
 * - Golden Section Search
 */

#pragma once

namespace math
{
static constexpr double GOLDEN_RATIO = 1.6180339887; //(sqrt(5)+1)/2

// Type-safe abs
template<typename _Tp>
_Tp abs_t(_Tp val)
{
	return ((val > (_Tp)0) ? val : -val);
}

// golden section search to find extremeum for function with minimum
template<typename _Tp>
inline const _Tp goldensection(const _Tp &arg1, const _Tp &arg2, _Tp(*fun)(_Tp), const _Tp &tol)
{
	_Tp a = arg1;
	_Tp b = arg2;
	_Tp c = b - (b - a) / GOLDEN_RATIO;
	_Tp d = a + (b - a) / GOLDEN_RATIO;

	while (abs_t(c - d) > tol) {

		if (fun(c) < fun(d)) {
			b = d;

		} else {
			a = c;
		}

		c = b - (b - a) / GOLDEN_RATIO;
		d = a + (b - a) / GOLDEN_RATIO;

	}

	return ((b + a) / (_Tp)2);
}
}
