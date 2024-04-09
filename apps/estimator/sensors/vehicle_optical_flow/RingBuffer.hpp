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
 * @file RingBuffer.hpp
 * Template RingBuffer
 */

#include <inttypes.h>
#include <cstdio>
#include <cstring>

template <typename T, size_t SIZE>
class RingBuffer {
public:
    void push(const T &sample) {
        uint8_t head_new = _head;

        if (!_first_write) {
            head_new = (_head + 1) % SIZE;
        }

        _buffer[head_new] = sample;
        _head             = head_new;

        // move tail if we overwrite it
        if (_head == _tail && !_first_write) {
            _tail = (_tail + 1) % SIZE;

        } else {
            _first_write = false;
        }
    }

    uint8_t get_length() const {
        return SIZE;
    }

    T &operator[](const uint8_t index) {
        return _buffer[index];
    }

    const T &get_newest() const {
        return _buffer[_head];
    }
    const T &get_oldest() const {
        return _buffer[_tail];
    }

    uint8_t get_oldest_index() const {
        return _tail;
    }

    bool peak_first_older_than(const uint64_t &timestamp, T *sample) {
        // start looking from newest observation data
        for (uint8_t i = 0; i < SIZE; i++) {
            int index = (_head - i);
            index     = index < 0 ? SIZE + index : index;

            if (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)100'000) {
                *sample = _buffer[index];
                return true;
            }

            if (index == _tail) {
                // we have reached the tail and haven't got a
                // match
                return false;
            }
        }

        return false;
    }

    bool pop_first_older_than(const uint64_t &timestamp, T *sample) {
        // start looking from newest observation data
        for (uint8_t i = 0; i < SIZE; i++) {
            int index = (_head - i);
            index     = index < 0 ? SIZE + index : index;

            if (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)100'000) {
                *sample = _buffer[index];

                // Now we can set the tail to the item which
                // comes after the one we removed since we don't
                // want to have any older data in the buffer
                if (index == _head) {
                    _tail        = _head;
                    _first_write = true;

                } else {
                    _tail = (index + 1) % SIZE;
                }

                _buffer[index].time_us = 0;

                return true;
            }

            if (index == _tail) {
                // we have reached the tail and haven't got a
                // match
                return false;
            }
        }

        return false;
    }

    bool pop_oldest(const uint64_t &timestamp_oldest, const uint64_t &timestamp_newest, T *sample) {
        if (timestamp_oldest >= timestamp_newest) {
            return false;
        }

        for (uint8_t i = 0; i < SIZE; i++) {
            uint8_t index = (_tail + i) % SIZE;

            if (_buffer[index].time_us >= timestamp_oldest && _buffer[index].time_us <= timestamp_newest) {
                *sample = _buffer[index];

                // Now we can set the tail to the item which
                // comes after the one we removed since we don't
                // want to have any older data in the buffer
                if (index == _head) {
                    _tail        = _head;
                    _first_write = true;

                } else {
                    _tail = (index + 1) % SIZE;
                }

                _buffer[index] = {};

                return true;
            }
        }

        return false;
    }

    bool pop_oldest(T *sample) {
        if (_tail == _head) {
            return false;
        }

        *sample                = _buffer[_tail];
        _buffer[_tail].time_us = 0;

        _tail = (_tail + 1) % SIZE;

        return true;
    }

    bool pop_newest(T *sample) {
        if (_tail == _head) {
            return false;
        }

        *sample                = _buffer[_head];
        _buffer[_head].time_us = 0;

        _head = (_head - 1) % SIZE;

        return true;
    }

    int get_total_size() const {
        return sizeof(*this) + sizeof(T) * SIZE;
    }

    uint8_t entries() const {
        int count = 0;

        for (uint8_t i = 0; i < SIZE; i++) {
            if (_buffer[i].time_us != 0) {
                count++;
            }
        }

        return count;
    }

private:
    T _buffer[SIZE]{};

    uint8_t _head{0};
    uint8_t _tail{0};

    bool _first_write{true};
};
