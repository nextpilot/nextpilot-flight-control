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
 * @file zero_velocity_update.cpp
 * Control function for ekf zero velocity update
 */

#include "ekf.h"

void Ekf::controlZeroVelocityUpdate() {
    // Fuse zero velocity at a limited rate (every 200 milliseconds)
    const bool zero_velocity_update_data_ready = isTimedOut(_time_last_zero_velocity_fuse, (uint64_t)2e5);

    if (zero_velocity_update_data_ready) {
        const bool continuing_conditions_passing = _control_status.flags.vehicle_at_rest && _control_status_prev.flags.vehicle_at_rest && !isVerticalVelocityAidingActive(); // otherwise the filter is "too rigid" to follow a position drift

        if (continuing_conditions_passing) {
            Vector3f vel_obs{0, 0, 0};
            Vector3f innovation = _state.vel - vel_obs;

            // Set a low variance initially for faster leveling and higher
            // later to let the states follow the measurements
            const float obs_var = _control_status.flags.tilt_align ? sq(0.2f) : sq(0.001f);
            Vector3f    innov_var{
                P(4, 4) + obs_var,
                P(5, 5) + obs_var,
                P(6, 6) + obs_var};

            fuseVelPosHeight(innovation(0), innov_var(0), 0);
            fuseVelPosHeight(innovation(1), innov_var(1), 1);
            fuseVelPosHeight(innovation(2), innov_var(2), 2);

            _time_last_zero_velocity_fuse = _time_delayed_us;
        }
    }
}
