/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <matrix/math.hpp>

#ifndef EKF_UTILS_HPP
#define EKF_UTILS_HPP

// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
// This function relies on the caller to be responsible for keeping a copy of
// "accumulator" and passing this value at the next iteration.
// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
inline float kahanSummation(float sum_previous, float input, float &accumulator) {
    const float y = input - accumulator;
    const float t = sum_previous + y;
    accumulator   = (t - sum_previous) - y;
    return t;
}

namespace ecl {
inline float powf(float x, int exp) {
    float ret;

    if (exp > 0) {
        ret = x;

        for (int count = 1; count < exp; count++) {
            ret *= x;
        }

        return ret;

    } else if (exp < 0) {
        return 1.0f / ecl::powf(x, -exp);
    }

    return 1.0f;
}

} // namespace ecl

#endif // EKF_UTILS_HPP
