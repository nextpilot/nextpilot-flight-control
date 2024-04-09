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
 * @file FixedwingLandDetector.cpp
 *
 * @author Johan Jansen <jnsn.johan@gmail.com>
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Julian Oes <julian@oes.ch>
 */

#include "FixedwingLandDetector.h"

namespace land_detector {

FixedwingLandDetector::FixedwingLandDetector() {
    // Use Trigger time when transitioning from in-air (false) to landed (true) / ground contact (true).
    _landed_hysteresis.set_hysteresis_time_from(false, _param_lndfw_trig_time.get() * 1_s);
    _landed_hysteresis.set_hysteresis_time_from(true, FLYING_TRIGGER_TIME_US);
}

bool FixedwingLandDetector::_get_landed_state() {
    // Only trigger flight conditions if we are armed.
    if (!_armed) {
        return true;
    }

    bool landDetected = false;

    launch_detection_status_s launch_detection_status{};
    _launch_detection_status_sub.copy(&launch_detection_status);

    // force the landed state to stay landed if we're currently in the catapult/hand-launch launch process. Detect that we are in this state
    // by checking if the last publication of launch_detection_status is less than 0.5s old, and we're not yet in the flying state.
    if (_landed_hysteresis.get_state() && hrt_elapsed_time(&launch_detection_status.timestamp) < 500_ms && launch_detection_status.launch_detection_state < launch_detection_status_s::STATE_FLYING) {
        landDetected = true;

    } else if (hrt_elapsed_time(&_vehicle_local_position.timestamp) < 1_s) {
        // Horizontal velocity complimentary filter.
        float val = 0.97f * _velocity_xy_filtered + 0.03f * sqrtf(_vehicle_local_position.vx * _vehicle_local_position.vx +
                                                                  _vehicle_local_position.vy * _vehicle_local_position.vy);

        if (PX4_ISFINITE(val)) {
            _velocity_xy_filtered = val;
        }

        // Vertical velocity complimentary filter.
        val = 0.99f * _velocity_z_filtered + 0.01f * fabsf(_vehicle_local_position.vz);

        if (PX4_ISFINITE(val)) {
            _velocity_z_filtered = val;
        }

        airspeed_validated_s airspeed_validated{};
        _airspeed_validated_sub.copy(&airspeed_validated);

        bool airspeed_invalid = false;

        // set _airspeed_filtered to 0 if airspeed data is invalid
        if (!PX4_ISFINITE(airspeed_validated.true_airspeed_m_s) || hrt_elapsed_time(&airspeed_validated.timestamp) > 1_s) {
            _airspeed_filtered = 0.0f;
            airspeed_invalid   = true;

        } else {
            _airspeed_filtered = 0.95f * _airspeed_filtered + 0.05f * airspeed_validated.true_airspeed_m_s;
        }

        // A leaking lowpass prevents biases from building up, but
        // gives a mostly correct response for short impulses.
        const float acc_hor = matrix::Vector2f(_acceleration).norm();
        _xy_accel_filtered  = _xy_accel_filtered * 0.8f + acc_hor * 0.18f;

        // make groundspeed threshold tighter if airspeed is invalid
        const float vel_xy_max_threshold = airspeed_invalid ? 0.7f * _param_lndfw_vel_xy_max.get() :
                                                              _param_lndfw_vel_xy_max.get();

        // Crude land detector for fixedwing.
        landDetected = _airspeed_filtered < _param_lndfw_airspd.get() && _velocity_xy_filtered < vel_xy_max_threshold && _velocity_z_filtered < _param_lndfw_vel_z_max.get() && _xy_accel_filtered < _param_lndfw_xyaccel_max.get();

    } else {
        // Control state topic has timed out and we need to assume we're landed.
        landDetected = true;
    }

    return landDetected;
}

} // namespace land_detector
