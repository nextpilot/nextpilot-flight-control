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
 * @file DataValidator.cpp
 *
 * A data validation class to identify anomalies in data streams
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#include "DataValidator.hpp"

#include <px4_platform_common/log.h>
#include <hrtimer.h>

void DataValidator::put(uint64_t timestamp, float val, uint32_t error_count_in, uint8_t priority_in) {
    float data[dimensions] = {val}; // sets the first value and all others to 0
    put(timestamp, data, error_count_in, priority_in);
}

void DataValidator::put(uint64_t timestamp, const float val[dimensions], uint32_t error_count_in, uint8_t priority_in) {
    _event_count++;

    if (error_count_in > _error_count) {
        _error_density += (error_count_in - _error_count);

    } else if (_error_density > 0) {
        _error_density--;
    }

    _error_count = error_count_in;
    _priority    = priority_in;

    for (unsigned i = 0; i < dimensions; i++) {
        if (PX4_ISFINITE(val[i])) {
            if (_time_last == 0) {
                _mean[i] = 0;
                _lp[i]   = val[i];
                _M2[i]   = 0;

            } else {
                float lp_val = val[i] - _lp[i];

                float delta_val = lp_val - _mean[i];
                _mean[i] += delta_val / _event_count;
                _M2[i] += delta_val * (lp_val - _mean[i]);
                _rms[i] = sqrtf(_M2[i] / (_event_count - 1));

                if (fabsf(_value[i] - val[i]) < 0.000001f) {
                    _value_equal_count++;

                } else {
                    _value_equal_count = 0;
                }
            }

            // XXX replace with better filter, make it auto-tune to update rate
            _lp[i] = _lp[i] * 0.99f + 0.01f * val[i];

            _value[i] = val[i];
        }
    }

    _time_last = timestamp;
}

float DataValidator::confidence(uint64_t timestamp) {
    float ret = 1.0f;

    /* check if we have any data */
    if (_time_last == 0) {
        _error_mask |= ERROR_FLAG_NO_DATA;
        ret = 0.0f;

    } else if (timestamp > _time_last + _timeout_interval) {
        /* timed out - that's it */
        _error_mask |= ERROR_FLAG_TIMEOUT;
        ret = 0.0f;

    } else if (_value_equal_count > _value_equal_count_threshold) {
        /* we got the exact same sensor value N times in a row */
        _error_mask |= ERROR_FLAG_STALE_DATA;
        ret = 0.0f;

    } else if (_error_count > NORETURN_ERRCOUNT) {
        /* check error count limit */
        _error_mask |= ERROR_FLAG_HIGH_ERRCOUNT;
        ret = 0.0f;

    } else if (_error_density > ERROR_DENSITY_WINDOW) {
        /* cap error density counter at window size */
        _error_mask |= ERROR_FLAG_HIGH_ERRDENSITY;
        _error_density = ERROR_DENSITY_WINDOW;
    }

    /* no critical errors */
    if (ret > 0.0f) {
        /* return local error density for last N measurements */
        ret = 1.0f - (_error_density / ERROR_DENSITY_WINDOW);

        if (ret > 0.0f) {
            _error_mask = ERROR_FLAG_NO_ERROR;
        }
    }

    return ret;
}

void DataValidator::print() {
    if (_time_last == 0) {
        PX4_INFO_RAW("\tno data\n");
        return;
    }

    for (unsigned i = 0; i < dimensions; i++) {
        PX4_INFO_RAW("\tval: %8.4f, lp: %8.4f mean dev: %8.4f RMS: %8.4f conf: %8.4f\n", (double)_value[i],
                     (double)_lp[i], (double)_mean[i], (double)_rms[i], (double)confidence(hrt_absolute_time()));
    }
}
