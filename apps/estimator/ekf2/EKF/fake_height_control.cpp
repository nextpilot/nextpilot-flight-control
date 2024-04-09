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
 * @file fake_height_control.cpp
 * Control functions for ekf fake height fusion
 */

#include "ekf.h"

void Ekf::controlFakeHgtFusion() {
    auto &aid_src = _aid_src_fake_hgt;

    // If we aren't doing any aiding, fake position measurements at the last known vertical position to constrain drift
    const bool fake_hgt_data_ready = !isVerticalAidingActive() && isTimedOut(aid_src.time_last_fuse, (uint64_t)2e5); // Fuse fake height at a limited rate

    if (fake_hgt_data_ready) {
        const float obs_var    = sq(_params.pos_noaid_noise);
        const float innov_gate = 3.f;

        updateVerticalPositionAidSrcStatus(_time_delayed_us, _last_known_pos(2), obs_var, innov_gate, aid_src);

        const bool continuing_conditions_passing = !isVerticalAidingActive();
        const bool starting_conditions_passing   = continuing_conditions_passing && _vertical_velocity_deadreckon_time_exceeded && _vertical_position_deadreckon_time_exceeded;

        if (_control_status.flags.fake_hgt) {
            if (continuing_conditions_passing) {
                // always protect against extreme values that could result in a NaN
                aid_src.fusion_enabled = aid_src.test_ratio < sq(100.0f / innov_gate);

                fuseVerticalPosition(aid_src);

                const bool is_fusion_failing = isTimedOut(aid_src.time_last_fuse, (uint64_t)4e5);

                if (is_fusion_failing) {
                    resetFakeHgtFusion();
                }

            } else {
                stopFakeHgtFusion();
            }

        } else {
            if (starting_conditions_passing) {
                ECL_INFO("start fake height fusion");
                _control_status.flags.fake_hgt = true;
                resetFakeHgtFusion();
            }
        }

    } else if (_control_status.flags.fake_hgt && isVerticalAidingActive()) {
        stopFakeHgtFusion();
    }
}

void Ekf::resetFakeHgtFusion() {
    ECL_INFO("reset fake height fusion");
    _last_known_pos(2) = _state.pos(2);

    resetVerticalVelocityToZero();
    resetHeightToLastKnown();

    _aid_src_fake_hgt.time_last_fuse = _time_delayed_us;
}

void Ekf::resetHeightToLastKnown() {
    _information_events.flags.reset_pos_to_last_known = true;
    ECL_INFO("reset height to last known (%.3f)", (double)_last_known_pos(2));
    resetVerticalPositionTo(_last_known_pos(2), sq(_params.pos_noaid_noise));
}

void Ekf::stopFakeHgtFusion() {
    if (_control_status.flags.fake_hgt) {
        ECL_INFO("stop fake height fusion");
        _control_status.flags.fake_hgt = false;

        resetEstimatorAidStatus(_aid_src_fake_hgt);
    }
}
