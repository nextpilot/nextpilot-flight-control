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
 * @file AlphaFilter.hpp
 *
 * @brief First order "alpha" IIR digital filter also known as leaky integrator or forgetting average.
 *
 * @author Mathieu Bresciani <brescianimathieu@gmail.com>
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include <float.h>
#include <mathlib/math/Functions.hpp>

using namespace math;

template <typename T>
class AlphaFilter
{
public:
	AlphaFilter() = default;
	explicit AlphaFilter(float alpha) : _alpha(alpha) {}

	~AlphaFilter() = default;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * Both parameters have to be provided in the same units.
	 *
	 * @param sample_interval interval between two samples
	 * @param time_constant filter time constant determining convergence
	 */
	void setParameters(float sample_interval, float time_constant)
	{
		const float denominator = time_constant + sample_interval;

		if (denominator > FLT_EPSILON) {
			setAlpha(sample_interval / denominator);
		}
	}

	bool setCutoffFreq(float sample_freq, float cutoff_freq)
	{
		if ((sample_freq <= 0.f) || (cutoff_freq <= 0.f) || (cutoff_freq >= sample_freq / 2.f)
		    || !isFinite(sample_freq) || !isFinite(cutoff_freq)) {

			// Invalid parameters
			return false;
		}

		setParameters(1.f / sample_freq, 1.f / (2.f * M_PI_F * cutoff_freq));
		_cutoff_freq = cutoff_freq;
		return true;
	}

	/**
	 * Set filter parameter alpha directly without time abstraction
	 *
	 * @param alpha [0,1] filter weight for the previous state. High value - long time constant.
	 */
	void setAlpha(float alpha) { _alpha = alpha; }

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	void reset(const T &sample) { _filter_state = sample; }

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	const T &update(const T &sample)
	{
		_filter_state = updateCalculation(sample);
		return _filter_state;
	}

	const T &getState() const { return _filter_state; }
	float getCutoffFreq() const { return _cutoff_freq; }

protected:
	T updateCalculation(const T &sample) { return (1.f - _alpha) * _filter_state + _alpha * sample; }

	float _cutoff_freq{0.f};
	float _alpha{0.f};
	T _filter_state{};
};
