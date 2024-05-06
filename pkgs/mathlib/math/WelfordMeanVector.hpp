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
 * @file WelfordMeanVector.hpp
 *
 * Welford's online algorithm for computing mean and covariance of a vector.
 */

#pragma once

#include <matrix/math.hpp>

namespace math
{

template <typename Type, size_t N>
class WelfordMeanVector
{
public:
	// For a new value, compute the new count, new mean, the new M2.
	bool update(const matrix::Vector<Type, N> &new_value)
	{
		if (_count == 0) {
			reset();
			_count = 1;
			_mean = new_value;
			return false;

		} else if (_count == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / _count;
			_M2_accum.zero();

			_count = 1;

		} else {
			_count++;
		}

		// mean
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		const matrix::Vector<Type, N> delta{new_value - _mean};
		{
			const matrix::Vector<Type, N> y = (delta / _count) - _mean_accum;
			const matrix::Vector<Type, N> t = _mean + y;
			_mean_accum = (t - _mean) - y;
			_mean = t;
		}

		if (!_mean.isAllFinite()) {
			reset();
			return false;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			matrix::SquareMatrix<Type, N> m2_change{};

			for (size_t r = 0; r < N; r++) {
				for (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_value(c) - _mean(c));
				}
			}

			for (size_t r = 0; r < N; r++) {
				for (size_t c = r; c < N; c++) {

					const Type y = m2_change(r, c) - _M2_accum(r, c);
					const Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				if (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			for (size_t r = 0; r < N; r++) {
				for (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		if (!_M2.isAllFinite()) {
			reset();
			return false;
		}

		return valid();
	}

	bool valid() const { return _count > 2; }
	auto count() const { return _count; }

	void reset()
	{
		_count = 0;
		_mean.zero();
		_M2.zero();

		_mean_accum.zero();
		_M2_accum.zero();
	}

	matrix::Vector<Type, N> mean() const { return _mean; }
	matrix::Vector<Type, N> variance() const { return _M2.diag() / (_count - 1); }
	matrix::SquareMatrix<Type, N> covariance() const { return _M2 / (_count - 1); }

	Type covariance(int x, int y) const { return _M2(x, y) / (_count - 1); }

private:

	matrix::Vector<Type, N> _mean{};
	matrix::Vector<Type, N> _mean_accum{};  ///< kahan summation algorithm accumulator for mean

	matrix::SquareMatrix<Type, N> _M2{};
	matrix::SquareMatrix<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t _count{0};
};

} // namespace math
