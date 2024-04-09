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
 * @file RingBuffer.h
 * @author Roman Bapst <bapstroman@gmail.com>
 * Template RingBuffer.
 */

#ifndef EKF_RINGBUFFER_H
#define EKF_RINGBUFFER_H

#include <inttypes.h>
#include <cstdio>
#include <cstring>

template <typename data_type>
class RingBuffer {
public:
    explicit RingBuffer(size_t size) {
        allocate(size);
    }
    RingBuffer() = delete;
    ~RingBuffer() {
        delete[] _buffer;
    }

    // no copy, assignment, move, move assignment
    RingBuffer(const RingBuffer &)            = delete;
    RingBuffer &operator=(const RingBuffer &) = delete;
    RingBuffer(RingBuffer &&)                 = delete;
    RingBuffer &operator=(RingBuffer &&)      = delete;

    bool allocate(uint8_t size) {
        if (valid() && (size == _size)) {
            // no change
            return true;
        }

        if (size == 0) {
            return false;
        }

        if (_buffer != nullptr) {
            delete[] _buffer;
        }

        _buffer = new data_type[size]{};

        if (_buffer == nullptr) {
            return false;
        }

        _size = size;

        reset();

        return true;
    }

    bool valid() const {
        return (_buffer != nullptr) && (_size > 0);
    }

    void push(const data_type &sample) {
        uint8_t head_new = _head;

        if (!_first_write) {
            head_new = (_head + 1) % _size;
        }

        _buffer[head_new] = sample;
        _head             = head_new;

        // move tail if we overwrite it
        if (_head == _tail && !_first_write) {
            _tail = (_tail + 1) % _size;

        } else {
            _first_write = false;
        }
    }

    uint8_t get_length() const {
        return _size;
    }

    data_type &operator[](const uint8_t index) {
        return _buffer[index];
    }

    const data_type &get_newest() const {
        return _buffer[_head];
    }
    const data_type &get_oldest() const {
        return _buffer[_tail];
    }

    uint8_t get_oldest_index() const {
        return _tail;
    }

    bool pop_first_older_than(const uint64_t &timestamp, data_type *sample) {
        // start looking from newest observation data
        for (uint8_t i = 0; i < _size; i++) {
            int index = (_head - i);
            index     = index < 0 ? _size + index : index;

            if (timestamp >= _buffer[index].time_us && timestamp < _buffer[index].time_us + (uint64_t)1e5) {
                *sample = _buffer[index];

                // Now we can set the tail to the item which
                // comes after the one we removed since we don't
                // want to have any older data in the buffer
                if (index == _head) {
                    _tail        = _head;
                    _first_write = true;

                } else {
                    _tail = (index + 1) % _size;
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

    int get_total_size() const {
        return sizeof(*this) + sizeof(data_type) * _size;
    }

    int entries() const {
        int count = 0;

        for (uint8_t i = 0; i < _size; i++) {
            if (_buffer[i].time_us != 0) {
                count++;
            }
        }

        return count;
    }

    void reset() {
        if (_buffer) {
            for (uint8_t i = 0; i < _size; i++) {
                _buffer[i] = {};
            }

            _head        = 0;
            _tail        = 0;
            _first_write = true;
        }
    }

private:
    data_type *_buffer{nullptr};

    uint8_t _head{0};
    uint8_t _tail{0};
    uint8_t _size{0};

    bool _first_write{true};
};

#endif // !EKF_RINGBUFFER_H
