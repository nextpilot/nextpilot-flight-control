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

#include <stdint.h>
#include <uORB/topics/manual_control_setpoint.h>

class ManualControlSelector {
public:
    void setRcInMode(int32_t rc_in_mode) {
        _rc_in_mode = rc_in_mode;
    }
    void setTimeout(uint64_t timeout) {
        _timeout = timeout;
    }
    void                       updateValidityOfChosenInput(uint64_t now);
    void                       updateWithNewInputSample(uint64_t now, const manual_control_setpoint_s &input, int instance);
    manual_control_setpoint_s &setpoint();
    int                        instance() const {
        return _instance;
    };

private:
    bool isInputValid(const manual_control_setpoint_s &input, uint64_t now) const;

    manual_control_setpoint_s _setpoint{};
    uint64_t                  _timeout{0};
    int32_t                   _rc_in_mode{0};
    int                       _instance{-1};
    uint8_t                   _first_valid_source{manual_control_setpoint_s::SOURCE_UNKNOWN};
};
