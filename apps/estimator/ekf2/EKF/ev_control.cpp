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
 * @file ev_control.cpp
 * Control functions for ekf external vision control
 */

#include "ekf.h"

void Ekf::controlExternalVisionFusion() {
    _ev_pos_b_est.predict(_dt_ekf_avg);

    // Check for new external vision data
    extVisionSample ev_sample;

    if (_ext_vision_buffer && _ext_vision_buffer->pop_first_older_than(_time_delayed_us, &ev_sample)) {
        bool ev_reset = (ev_sample.reset_counter != _ev_sample_prev.reset_counter);

        // determine if we should use the horizontal position observations
        bool quality_sufficient = (_params.ev_quality_minimum <= 0) || (ev_sample.quality >= _params.ev_quality_minimum);

        const bool starting_conditions_passing = quality_sufficient && ((ev_sample.time_us - _ev_sample_prev.time_us) < EV_MAX_INTERVAL) && ((_params.ev_quality_minimum <= 0) || (_ev_sample_prev.quality >= _params.ev_quality_minimum)) // previous quality sufficient
                                                 && ((_params.ev_quality_minimum <= 0) || (_ext_vision_buffer->get_newest().quality >= _params.ev_quality_minimum))                                                                        // newest quality sufficient
                                                 && isNewestSampleRecent(_time_last_ext_vision_buffer_push, EV_MAX_INTERVAL);

        controlEvYawFusion(ev_sample, starting_conditions_passing, ev_reset, quality_sufficient, _aid_src_ev_yaw);
        controlEvVelFusion(ev_sample, starting_conditions_passing, ev_reset, quality_sufficient, _aid_src_ev_vel);
        controlEvPosFusion(ev_sample, starting_conditions_passing, ev_reset, quality_sufficient, _aid_src_ev_pos);
        controlEvHeightFusion(ev_sample, starting_conditions_passing, ev_reset, quality_sufficient, _aid_src_ev_hgt);

        if (quality_sufficient) {
            _ev_sample_prev = ev_sample;
        }

        // record corresponding yaw state for future EV delta heading innovation (logging only)
        _ev_yaw_pred_prev = getEulerYaw(_state.quat_nominal);

    } else if ((_control_status.flags.ev_pos || _control_status.flags.ev_vel || _control_status.flags.ev_yaw || _control_status.flags.ev_hgt) && isTimedOut(_ev_sample_prev.time_us, 2 * EV_MAX_INTERVAL)) {
        // Turn off EV fusion mode if no data has been received
        stopEvPosFusion();
        stopEvVelFusion();
        stopEvYawFusion();
        stopEvHgtFusion();

        _warning_events.flags.vision_data_stopped = true;
        ECL_WARN("vision data stopped");
    }
}
