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

#include <hrtimer.h>

/**
 * @class TimestampedList
 */
template <class T, int NUM_ITEMS>
class TimestampedList {
public:
    TimestampedList()  = default;
    ~TimestampedList() = default;

    /**
     * Insert a value into the list, overwrite the oldest entry if full.
     */
    void put(const T &new_value) {
        hrt_abstime now = hrt_absolute_time();

        // Insert it wherever there is a free space.
        for (int i = 0; i < NUM_ITEMS; ++i) {
            if (_list[i].timestamp_us == 0) {
                _list[i].timestamp_us = now;
                _list[i].value        = new_value;
                return;
            }
        }

        // Find oldest entry.
        int oldest_i = 0;

        for (int i = 1; i < NUM_ITEMS; ++i) {
            if (_list[i].timestamp_us < _list[oldest_i].timestamp_us) {
                oldest_i = i;
            }
        }

        // And overwrite oldest.
        _list[oldest_i].timestamp_us = now;
        _list[oldest_i].value        = new_value;
    }

    /**
     * Before iterating using get_next(), reset to start.
     */
    void reset_to_start() {
        _current_i = -1;
    }

    /**
     * Iterate through all active values (not sorted).
     * Return nullptr if at end of list.
     *
     * This is basically a poor man's iterator.
     */
    T *get_next() {
        // Increment first, then leave it until called again.
        ++_current_i;

        for (int i = _current_i; i < NUM_ITEMS; ++i) {
            if (_list[i].timestamp_us != 0) {
                _current_i = i;
                return &_list[i].value;
            }
        }

        return nullptr;
    }

    /**
     * Disable the last item that we have gotten.
     */
    void drop_current() {
        if (_current_i < NUM_ITEMS) {
            _list[_current_i].timestamp_us = 0;
        }
    }

    /**
     * Update the timestamp of the item we have gotten.
     */
    void update_current() {
        if (_current_i < NUM_ITEMS) {
            _list[_current_i].timestamp = hrt_absolute_time();
        }
    }

private:
    struct item_s {
        hrt_abstime timestamp_us = 0; // 0 signals inactive.
        T           value{};
    };

    item_s _list[NUM_ITEMS]{};

    int _current_i{-1};
};
