/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "ManualControlSelector"
#define LOG_LVL LOG_LVL_INFO


#include "ManualControlSelector.hpp"

void ManualControlSelector::updateValidityOfChosenInput(uint64_t now) {
    if (!isInputValid(_setpoint, now)) {
        _setpoint.valid = false;
        _instance       = -1;
    }
}

void ManualControlSelector::updateWithNewInputSample(uint64_t now, const manual_control_setpoint_s &input, int instance) {
    // First check if the chosen input got invalid, so it can get replaced
    updateValidityOfChosenInput(now);

    const bool update_existing_input = _setpoint.valid && input.data_source == _setpoint.data_source;
    const bool start_using_new_input = !_setpoint.valid;

    // Switch to new input if it's valid and we don't already have a valid one
    bool ret = isInputValid(input, now);
    rt_kprintf("%d, %d, %d\n", ret, update_existing_input, start_using_new_input); // 1 1 0

    if (isInputValid(input, now) && (update_existing_input || start_using_new_input)) {
        _setpoint           = input;
        _setpoint.valid     = true;
        _setpoint.timestamp = now; // timestamp_sample is preserved
        _instance           = instance;

        if (_first_valid_source == manual_control_setpoint_s::SOURCE_UNKNOWN) {
            // initialize first valid source once
            _first_valid_source = _setpoint.data_source;
        }
    }
}

bool ManualControlSelector::isInputValid(const manual_control_setpoint_s &input, uint64_t now) const {
    // Check for timeout
    const bool sample_from_the_past      = now >= input.timestamp_sample;
    const bool sample_newer_than_timeout = now - input.timestamp_sample < _timeout;

    // Check if source matches the configuration
    const bool source_rc_matched      = (_rc_in_mode == 0) && (input.data_source == manual_control_setpoint_s::SOURCE_RC);
    const bool source_mavlink_matched = (_rc_in_mode == 1) && (input.data_source == manual_control_setpoint_s::SOURCE_MAVLINK_0 || input.data_source == manual_control_setpoint_s::SOURCE_MAVLINK_1 || input.data_source == manual_control_setpoint_s::SOURCE_MAVLINK_2 || input.data_source == manual_control_setpoint_s::SOURCE_MAVLINK_3 || input.data_source == manual_control_setpoint_s::SOURCE_MAVLINK_4 || input.data_source == manual_control_setpoint_s::SOURCE_MAVLINK_5);
    const bool source_any_matched     = (_rc_in_mode == 2);
    const bool source_first_matched   = (_rc_in_mode == 3) && (input.data_source == _first_valid_source || _first_valid_source == manual_control_setpoint_s::SOURCE_UNKNOWN);

    // // 正常情况下返回 true
    // LOG_I("all ret: %d, %d, %d, %d, %d, %d",
    //       sample_from_the_past,      // ok
    //       sample_newer_than_timeout, // error
    //       source_rc_matched,
    //       source_mavlink_matched,
    //       source_any_matched,
    //       source_first_matched);

    // LOG_I("now: %llu, input.timestamp_sample: %llu, _timeout: %llu", now, input.timestamp_sample, _timeout);

    return (sample_from_the_past && sample_newer_than_timeout
            && (source_rc_matched || source_mavlink_matched || source_any_matched || source_first_matched));
}

manual_control_setpoint_s &ManualControlSelector::setpoint() {
    return _setpoint;
}
