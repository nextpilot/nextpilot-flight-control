/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/// @file	LowPassFilter2p.hpp
/// @brief	A class to implement a second order low pass filter
/// Author: Leonard Hall <LeonardTHall@gmail.com>
/// Adapted for PX4 by Andrew Tridgell

#pragma once

#include <mathlib/math/Functions.hpp>
#include <float.h>
#include <matrix/math.hpp>

namespace math
{

template<typename T>
class LowPassFilter2p
{
public:
	LowPassFilter2p() = default;

	LowPassFilter2p(float sample_freq, float cutoff_freq)
	{
		// set initial parameters
		set_cutoff_frequency(sample_freq, cutoff_freq);
	}

	// Change filter parameters
	void set_cutoff_frequency(float sample_freq, float cutoff_freq)
	{
		if ((sample_freq <= 0.f) || (cutoff_freq <= 0.f) || (cutoff_freq >= sample_freq / 2)
		    || !isFinite(sample_freq) || !isFinite(cutoff_freq)) {

			disable();
			return;
		}

		// reset delay elements on filter change
		_delay_element_1 = {};
		_delay_element_2 = {};

		_cutoff_freq = math::max(cutoff_freq, sample_freq * 0.001f);
		_sample_freq = sample_freq;

		const float fr = _sample_freq / _cutoff_freq;
		const float ohm = tanf(M_PI_F / fr);
		const float c = 1.f + 2.f * cosf(M_PI_F / 4.f) * ohm + ohm * ohm;

		_b0 = ohm * ohm / c;
		_b1 = 2.f * _b0;
		_b2 = _b0;

		_a1 = 2.f * (ohm * ohm - 1.f) / c;
		_a2 = (1.f - 2.f * cosf(M_PI_F / 4.f) * ohm + ohm * ohm) / c;

		if (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a1) || !isFinite(_a2)) {
			disable();
		}
	}

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	inline T apply(const T &sample)
	{
		// Direct Form II implementation
		T delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};

		const T output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

		_delay_element_2 = _delay_element_1;
		_delay_element_1 = delay_element_0;

		return output;
	}

	// Filter array of samples in place using the Direct form II.
	inline void applyArray(T samples[], int num_samples)
	{
		for (int n = 0; n < num_samples; n++) {
			samples[n] = apply(samples[n]);
		}
	}

	// Return the cutoff frequency
	float get_cutoff_freq() const { return _cutoff_freq; }

	// Return the sample frequency
	float get_sample_freq() const { return _sample_freq; }

	float getMagnitudeResponse(float frequency) const;

	// Reset the filter state to this value
	T reset(const T &sample)
	{
		const T input = isFinite(sample) ? sample : T{};

		if (fabsf(1 + _a1 + _a2) > FLT_EPSILON) {
			_delay_element_1 = _delay_element_2 = input / (1 + _a1 + _a2);

			if (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
				_delay_element_1 = _delay_element_2 = input;
			}

		} else {
			_delay_element_1 = _delay_element_2 = input;
		}

		return apply(input);
	}

	void disable()
	{
		// no filtering
		_sample_freq = 0.f;
		_cutoff_freq = 0.f;

		_delay_element_1 = {};
		_delay_element_2 = {};

		_b0 = 1.f;
		_b1 = 0.f;
		_b2 = 0.f;

		_a1 = 0.f;
		_a2 = 0.f;
	}

protected:
	T _delay_element_1{}; // buffered sample -1
	T _delay_element_2{}; // buffered sample -2

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	float _a1{0.f};
	float _a2{0.f};

	float _b0{1.f};
	float _b1{0.f};
	float _b2{0.f};

	float _cutoff_freq{0.f};
	float _sample_freq{0.f};
};

} // namespace math
