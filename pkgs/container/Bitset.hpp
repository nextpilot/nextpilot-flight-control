/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <stdint.h>
#include <stddef.h>

// namespace px4
//{

template <size_t N>
class Bitset {
public:
    size_t count() const {
        size_t total = 0;

        for (auto x : _data) {
            while (x) {
                total += x & 1;
                x >>= 1;
            }
        }

        return total;
    }

    size_t size() const {
        return N;
    }

    bool operator[](size_t position) const {
        return _data[array_index(position)] & element_mask(position);
    }

    void set(size_t pos, bool val = true) {
        const uint8_t bitmask = element_mask(pos);

        if (val) {
            _data[array_index(pos)] |= bitmask;

        } else {
            _data[array_index(pos)] &= ~bitmask;
        }
    }

private:
    static constexpr uint8_t BITS_PER_ELEMENT = 8;
    static constexpr size_t  ARRAY_SIZE       = (N % BITS_PER_ELEMENT == 0) ? N / BITS_PER_ELEMENT : N / BITS_PER_ELEMENT + 1;
    static constexpr size_t  ALLOCATED_BITS   = ARRAY_SIZE * BITS_PER_ELEMENT;

    size_t array_index(size_t position) const {
        return position / BITS_PER_ELEMENT;
    }
    uint8_t element_mask(size_t position) const {
        return (1 << position % BITS_PER_ELEMENT);
    }

    uint8_t _data[ARRAY_SIZE]{};
};

//} // namespace px4
