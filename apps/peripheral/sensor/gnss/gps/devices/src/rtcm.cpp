/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "rtcm.h"
#include <cstring>

RTCMParsing::RTCMParsing() {
    reset();
}

RTCMParsing::~RTCMParsing() {
    delete[] _buffer;
}

void RTCMParsing::reset() {
    if (!_buffer) {
        _buffer     = new uint8_t[RTCM_INITIAL_BUFFER_LENGTH];
        _buffer_len = RTCM_INITIAL_BUFFER_LENGTH;
    }

    _pos            = 0;
    _message_length = _buffer_len;
}

bool RTCMParsing::addByte(uint8_t b) {
    if (!_buffer) {
        return false;
    }

    _buffer[_pos++] = b;

    if (_pos == 3) {
        _message_length = (((uint16_t)_buffer[1] & 3) << 8) | (_buffer[2]);

        if (_message_length + 6 > _buffer_len) {
            uint16_t new_buffer_len = _message_length + 6;
            uint8_t *new_buffer     = new uint8_t[new_buffer_len];

            if (!new_buffer) {
                delete[] (_buffer);
                _buffer = nullptr;
                return false;
            }

            memcpy(new_buffer, _buffer, 3);
            delete[] (_buffer);
            _buffer     = new_buffer;
            _buffer_len = new_buffer_len;
        }
    }

    return _message_length + 6 == _pos;
}
