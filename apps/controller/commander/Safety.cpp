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
 * @file Safety.cpp
 */

#include "Safety.hpp"
#include <circuit_breaker/circuit_breaker.h>

using namespace time_literals;

Safety::Safety() {
    // Safety can be turned off with the CBRK_IO_SAFETY parameter.
    _safety_disabled = circuit_breaker_enabled("CBRK_IO_SAFETY", CBRK_IO_SAFETY_KEY);

    if (_safety_disabled) {
        _button_available = true;
        _safety_off       = true;
    }
}

bool Safety::safetyButtonHandler() {
    if (!_safety_disabled) {
        if (!_button_available && _safety_button_sub.advertised()) {
            _button_available = true;
        }

        button_event_s button_event;

        while (_safety_button_sub.update(&button_event)) {
            _safety_off |= button_event.triggered; // triggered safety button activates safety off
        }
    }

    const bool safety_changed = _previous_safety_off != _safety_off;
    _previous_safety_off      = _safety_off;
    return safety_changed;
}

void Safety::activateSafety() {
    if (!_safety_disabled) {
        _safety_off = false;
    }
}
