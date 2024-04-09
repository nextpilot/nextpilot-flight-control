/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <mathlib/math/filter/AlphaFilter.hpp>

class MovingDiff {
public:
    float update(float value, float dt_s) {
        if (!PX4_ISFINITE(value) || (dt_s < FLT_EPSILON)) {
            // Ignore NAN
            return NAN;
        }

        _difference_filter.setParameters(dt_s, .1f);

        // Leave _diff at 0.0f if we don't have a _last_value yet.
        if (PX4_ISFINITE(_last_value)) {
            const float new_diff = (value - _last_value) / dt_s;
            _difference_filter.update(new_diff);
        }

        _last_value = value;
        return _difference_filter.getState();
    }

    void reset() {
        _difference_filter.reset(0.f);
        _last_value = NAN;
    }

private:
    AlphaFilter<float> _difference_filter;
    float              _last_value{NAN};
};
