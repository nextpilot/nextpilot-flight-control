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

class UnicoreParser {
public:
    enum class Result {
        None,
        WrongCrc,
        WrongStructure,
        GotHeading,
        GotAgrica,
        UnknownSentence,
    };

    Result parseChar(char c);

    struct Heading {
        float heading_deg;
        float heading_stddev_deg;
        float baseline_m;
    };

    struct Agrica {
    };

    Heading heading() const {
        return _heading;
    }

    Agrica agrica() const {
        return _agrica;
    }


private:
    void reset();
    bool crcCorrect() const;
    bool isHeading() const;
    bool isAgrica() const;
    bool extractHeading();
    bool extractAgrica();

    // We have seen buffers with 540 bytes for AGRICA.
    char     _buffer[600];
    unsigned _buffer_pos{0};
    char     _buffer_crc[9];
    unsigned _buffer_crc_pos{0};

    enum class State {
        Uninit,
        GotHashtag,
        GotStar,
    } _state{State::Uninit};

    Heading _heading{};
    Agrica  _agrica{};
};
