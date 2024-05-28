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
 * @file Safety.hpp
 */

#pragma once

#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/button_event.h>

using namespace nextpilot;

class Safety {
public:
    Safety();
    ~Safety() = default;

    bool safetyButtonHandler();
    void activateSafety();

    bool isButtonAvailable() const {
        return _button_available;
    }

    bool isSafetyOff() const {
        return _safety_off;
    }

    bool isSafetyDisabled() const {
        return _safety_disabled;
    }

private:
    uORB::Subscription _safety_button_sub{ORB_ID::safety_button};

    bool _button_available{false};    ///< Set to true if a safety button is connected
    bool _safety_off{false};          ///< Set to true if safety is off
    bool _previous_safety_off{false}; ///< Previous safety value
    bool _safety_disabled{false};     ///< Set to true if safety is disabled
};
