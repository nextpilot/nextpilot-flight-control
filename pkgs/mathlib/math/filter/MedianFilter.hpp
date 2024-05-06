/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/*
 * @file MedianFilter.hpp
 *
 * @brief Implementation of a median filter with C qsort.
 *
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

namespace math
{

template<typename T, int WINDOW = 3>
class MedianFilter
{
public:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = default;

	void insert(const T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, sizeof(_buffer));
		qsort(&sorted, WINDOW, sizeof(T), cmp);

		return sorted[WINDOW / 2];
	}

	T apply(const T &sample)
	{
		insert(sample);
		return median();
	}

private:

	static int cmp(const void *a, const void *b)
	{
		return (*(T *)a >= *(T *)b) ? 1 : -1;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
