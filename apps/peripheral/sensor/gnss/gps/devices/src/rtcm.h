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

#include <cstdint>

/* RTCM3 */
#define RTCM3_PREAMBLE             0xD3
#define RTCM_INITIAL_BUFFER_LENGTH 300 /**< initial maximum message length of an RTCM message */

class RTCMParsing {
public:
    RTCMParsing();
    ~RTCMParsing();

    /**
	 * reset the parsing state
	 */
    void reset();

    /**
	 * add a byte to the message
	 * @param b
	 * @return true if message complete (use @message to get it)
	 */
    bool addByte(uint8_t b);

    uint8_t *message() const {
        return _buffer;
    }

    uint16_t messageLength() const {
        return _pos;
    }

    uint16_t messageId() const {
        return (_buffer[3] << 4) | (_buffer[4] >> 4);
    }

private:
    uint8_t *_buffer{nullptr};
    uint16_t _buffer_len{};
    uint16_t _pos{};            ///< next position in buffer
    uint16_t _message_length{}; ///< message length without header & CRC (both 3 bytes)
};
