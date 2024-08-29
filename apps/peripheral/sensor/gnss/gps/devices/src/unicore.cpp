/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "unicore.h"
#include "crc.h"
#include <cstdio>
#include <stdlib.h>
#include <string.h>

UnicoreParser::Result UnicoreParser::parseChar(char c) {
    switch (_state) {
    case State::Uninit:
        if (c == '#') {
            _state = State::GotHashtag;
        }

        break;

    case State::GotHashtag:
        if (c == '*') {
            _state = State::GotStar;

            // Make sure buffer is zero terminated.
            _buffer[_buffer_pos] = '\0';

        } else {
            if (_buffer_pos >= sizeof(_buffer) - 1) {
                reset();
                return Result::None;
            }

            _buffer[_buffer_pos++] = c;
        }

        break;

    case State::GotStar:
        _buffer_crc[_buffer_crc_pos++] = c;

        if (_buffer_crc_pos >= 8) {
            // Make sure the CRC buffer is zero terminated.
            _buffer_crc[_buffer_crc_pos] = '\0';

            if (!crcCorrect()) {
                return Result::WrongCrc;
            }

            if (isHeading()) {
                if (extractHeading()) {
                    reset();
                    return Result::GotHeading;

                } else {
                    reset();
                    return Result::WrongStructure;
                }

            } else if (isAgrica()) {
                if (extractAgrica()) {
                    reset();
                    return Result::GotAgrica;

                } else {
                    reset();
                    return Result::WrongStructure;
                }

            } else {
                reset();
                return Result::UnknownSentence;
            }
        }

        break;
    }

    return Result::None;
}

void UnicoreParser::reset() {
    _state          = State::Uninit;
    _buffer_pos     = 0;
    _buffer_crc_pos = 0;
}

bool UnicoreParser::crcCorrect() const {
    const uint32_t crc_calculated = calculateCRC32(_buffer_pos, (uint8_t *)_buffer, 0);
    const uint32_t crc_parsed     = (uint32_t)strtoul(_buffer_crc, nullptr, 16);
    return (crc_calculated == crc_parsed);
}

bool UnicoreParser::isHeading() const {
    const char header[] = "UNIHEADINGA";

    return strncmp(header, _buffer, strlen(header)) == 0;
}

bool UnicoreParser::isAgrica() const {
    const char header[] = "AGRICA";

    return strncmp(header, _buffer, strlen(header)) == 0;
}

bool UnicoreParser::extractHeading() {
    // The basline starts after ;,, and then follows the heading.

    // Skip to ;
    char *ptr = strchr(_buffer, ';');

    if (ptr == nullptr) {
        return false;
    }

    ptr = strtok(ptr, ",");

    unsigned i = 0;

    while (ptr != nullptr) {
        ptr = strtok(nullptr, ",");

        if (ptr == nullptr) {
            return false;
        }

        if (i == 1) {
            _heading.baseline_m = strtof(ptr, nullptr);

        } else if (i == 2) {
            _heading.heading_deg = strtof(ptr, nullptr);

        } else if (i == 5) {
            _heading.heading_stddev_deg = strtof(ptr, nullptr);
            return true;
        }

        ++i;
    }

    return false;
}

bool UnicoreParser::extractAgrica() {
    return true;
}
