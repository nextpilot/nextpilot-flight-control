/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include "mavlink_simple_analyzer.h"

#include <float.h>

#include <ulog/log.h>
#include <defines.h>

SimpleAnalyzer::SimpleAnalyzer(Mode mode, float window) :
    _window(window),
    _mode(mode) {
    reset();
}

void SimpleAnalyzer::reset() {
    _n = 0;

    switch (_mode) {
    case AVERAGE:
        _result = 0.0f;

        break;

    case MIN:
        _result = FLT_MAX;

        break;

    case MAX:
        _result = FLT_MIN;

        break;

    default:
        PX4_ERR("SimpleAnalyzer: Unknown mode.");
    }
}

void SimpleAnalyzer::add_value(float val, float update_rate) {
    switch (_mode) {
    case AVERAGE:
        _result = (_result * _n + val) / (_n + 1u);

        break;

    case MIN:
        if (val < _result) {
            _result = val;
        }

        break;

    case MAX:
        if (val > _result) {
            _result = val;
        }

        break;
    }

    // if we get more measurements than n_max so the exponential moving average
    // is computed
    if ((_n < update_rate * _window) && (update_rate > 1.0f)) {
        _n++;
    }

    // value sanity checks
    if (!PX4_ISFINITE(_result)) {
        PX4_DEBUG("SimpleAnalyzer: Result is not finite, reset the analyzer.");
        reset();
    }
}

bool SimpleAnalyzer::valid() const {
    return _n > 0u;
}

float SimpleAnalyzer::get() const {
    return _result;
}

float SimpleAnalyzer::get_scaled(float scalingfactor) const {
    return get() * scalingfactor;
}

void SimpleAnalyzer::check_limits(float &x, float min, float max) const {
    if (x > max) {
        x = max;

    } else if (x < min) {
        x = min;
    }
}

void SimpleAnalyzer::int_round(float &x) const {
    if (x < 0) {
        x -= 0.5f;

    } else {
        x += 0.5f;
    }
}

void convert_limit_safe(float in, uint16_t &out) {
    if (in > UINT16_MAX) {
        out = UINT16_MAX;

    } else if (in < 0) {
        out = 0;

    } else {
        out = static_cast<uint16_t>(in);
    }
}

void convert_limit_safe(float in, int16_t &out) {
    if (in > INT16_MAX) {
        out = INT16_MAX;

    } else if (in < INT16_MIN) {
        out = INT16_MIN;

    } else {
        out = static_cast<int16_t>(in);
    }
}
