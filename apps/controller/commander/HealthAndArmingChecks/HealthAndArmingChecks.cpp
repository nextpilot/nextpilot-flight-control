/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "HealthAndArmingChecks.hpp"

HealthAndArmingChecks::HealthAndArmingChecks(ModuleParams *parent, vehicle_status_s &status) :
    ModuleParams(parent),
    _context(status) {
    // Initialize mode requirements to invalid
    _failsafe_flags.angular_velocity_invalid       = true;
    _failsafe_flags.attitude_invalid               = true;
    _failsafe_flags.local_altitude_invalid         = true;
    _failsafe_flags.local_position_invalid         = true;
    _failsafe_flags.local_position_invalid_relaxed = true;
    _failsafe_flags.local_velocity_invalid         = true;
    _failsafe_flags.global_position_invalid        = true;
    _failsafe_flags.auto_mission_missing           = true;
    _failsafe_flags.offboard_control_signal_lost   = true;
    _failsafe_flags.home_position_invalid          = true;
}

bool HealthAndArmingChecks::update(bool force_reporting) {
    _reporter.reset();

    _reporter.prepare(_context.status().vehicle_type);

    for (unsigned i = 0; i < sizeof(_checks) / sizeof(_checks[0]); ++i) {
        if (!_checks[i]) {
            break;
        }

        _checks[i]->checkAndReport(_context, _reporter);
    }

    const bool results_changed = _reporter.finalize();
    const bool reported        = _reporter.report(_context.isArmed(), force_reporting);

    if (reported) {
        // LEGACY start
        // Run the checks again, this time with the mavlink publication set.
        // We don't expect any change, and rate limitation would prevent the events from being reported again,
        // so we only report mavlink_log_*.
        _reporter._mavlink_log_pub = &_mavlink_log_pub;
        _reporter.reset();

        _reporter.prepare(_context.status().vehicle_type);

        for (unsigned i = 0; i < sizeof(_checks) / sizeof(_checks[0]); ++i) {
            if (!_checks[i]) {
                break;
            }

            _checks[i]->checkAndReport(_context, _reporter);
        }

        _reporter.finalize();
        _reporter.report(_context.isArmed(), false);
        _reporter._mavlink_log_pub = nullptr;
        // LEGACY end

        health_report_s health_report;
        _reporter.getHealthReport(health_report);
        health_report.timestamp = hrt_absolute_time();
        _health_report_pub.publish(health_report);
    }

    // Check if we need to publish the failsafe flags
    const hrt_abstime now = hrt_absolute_time();

    if ((now > _failsafe_flags.timestamp + 500_ms) || results_changed) {
        _failsafe_flags.timestamp = hrt_absolute_time();
        _failsafe_flags_pub.publish(_failsafe_flags);
    }

    return reported;
}

void HealthAndArmingChecks::updateParams() {
    for (unsigned i = 0; i < sizeof(_checks) / sizeof(_checks[0]); ++i) {
        if (!_checks[i]) {
            break;
        }

        _checks[i]->updateParams();
    }
}
