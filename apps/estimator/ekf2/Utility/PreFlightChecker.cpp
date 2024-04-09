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
 * @file PreFlightCheckHelper.cpp
 * Class handling the EKF2 innovation pre flight checks
 */

#include "PreFlightChecker.hpp"

void PreFlightChecker::update(const float dt, const estimator_innovations_s &innov) {
    const float alpha = InnovationLpf::computeAlphaFromDtAndTauInv(dt, _innov_lpf_tau_inv);

    _has_heading_failed   = preFlightCheckHeadingFailed(innov, alpha);
    _has_horiz_vel_failed = preFlightCheckHorizVelFailed(innov, alpha);
    _has_vert_vel_failed  = preFlightCheckVertVelFailed(innov, alpha);
    _has_height_failed    = preFlightCheckHeightFailed(innov, alpha);
}

bool PreFlightChecker::preFlightCheckHeadingFailed(const estimator_innovations_s &innov, const float alpha) {
    const float heading_test_limit      = selectHeadingTestLimit();
    const float heading_innov_spike_lim = 2.0f * heading_test_limit;

    const float heading_innov_lpf = _filter_heading_innov.update(innov.heading, alpha, heading_innov_spike_lim);

    return checkInnovFailed(heading_innov_lpf, innov.heading, heading_test_limit, heading_innov_spike_lim);
}

float PreFlightChecker::selectHeadingTestLimit() {
    // Select the max allowed heading innovaton depending on whether we are not aiding navigation using
    // observations in the NE reference frame and if the vehicle can use GPS course to realign in flight (fixedwing sideslip fusion).
    const bool is_ne_aiding = _is_using_gps_aiding || _is_using_ev_pos_aiding;

    return (is_ne_aiding && !_can_observe_heading_in_flight) ? _nav_heading_innov_test_lim // more restrictive test limit
                                                               :
                                                               _heading_innov_test_lim; // less restrictive test limit
}

bool PreFlightChecker::preFlightCheckHorizVelFailed(const estimator_innovations_s &innov, const float alpha) {
    bool has_failed = false;

    if (_is_using_gps_aiding || _is_using_ev_vel_aiding) {
        const Vector2f vel_ne_innov = Vector2f(fmaxf(fabsf(innov.gps_hvel[0]), fabsf(innov.ev_hvel[0])),
                                               fmaxf(fabsf(innov.gps_hvel[1]), fabsf(innov.ev_hvel[1])));
        Vector2f       vel_ne_innov_lpf;
        vel_ne_innov_lpf(0) = _filter_vel_n_innov.update(vel_ne_innov(0), alpha, _vel_innov_spike_lim);
        vel_ne_innov_lpf(1) = _filter_vel_e_innov.update(vel_ne_innov(1), alpha, _vel_innov_spike_lim);
        has_failed |= checkInnov2DFailed(vel_ne_innov_lpf, vel_ne_innov, _vel_innov_test_lim, _vel_innov_spike_lim);
    }

#if defined(CONFIG_EKF2_OPTICAL_FLOW)

    if (_is_using_flow_aiding) {
        const Vector2f flow_innov = Vector2f(innov.flow) * _flow_dist_bottom;
        Vector2f       flow_innov_lpf;
        flow_innov_lpf(0) = _filter_flow_x_innov.update(flow_innov(0), alpha, _flow_innov_spike_lim);
        flow_innov_lpf(1) = _filter_flow_y_innov.update(flow_innov(1), alpha, _flow_innov_spike_lim);
        has_failed |= checkInnov2DFailed(flow_innov_lpf, flow_innov, _flow_innov_test_lim, 5.f * _flow_innov_spike_lim);
    }

#endif // CONFIG_EKF2_OPTICAL_FLOW
    return has_failed;
}

bool PreFlightChecker::preFlightCheckVertVelFailed(const estimator_innovations_s &innov, const float alpha) {
    const float vel_d_innov     = fmaxf(fabsf(innov.gps_vvel), fabs(innov.ev_vvel)); // only temporary solution
    const float vel_d_innov_lpf = _filter_vel_d_innov.update(vel_d_innov, alpha, _vel_innov_spike_lim);
    return checkInnovFailed(vel_d_innov_lpf, vel_d_innov, _vel_innov_test_lim, _vel_innov_spike_lim);
}

bool PreFlightChecker::preFlightCheckHeightFailed(const estimator_innovations_s &innov, const float alpha) {
    bool has_failed = false;

    if (_is_using_baro_hgt_aiding) {
        const float baro_hgt_innov_lpf = _filter_baro_hgt_innov.update(innov.baro_vpos, alpha, _hgt_innov_spike_lim);
        has_failed |= checkInnovFailed(baro_hgt_innov_lpf, innov.baro_vpos, _hgt_innov_test_lim, _hgt_innov_spike_lim);
    }

    if (_is_using_gps_hgt_aiding) {
        const float gps_hgt_innov_lpf = _filter_gps_hgt_innov.update(innov.gps_vpos, alpha, _hgt_innov_spike_lim);
        has_failed |= checkInnovFailed(gps_hgt_innov_lpf, innov.gps_vpos, _hgt_innov_test_lim, _hgt_innov_spike_lim);
    }

#if defined(CONFIG_EKF2_RANGE_FINDER)

    if (_is_using_rng_hgt_aiding) {
        const float rng_hgt_innov_lpf = _filter_rng_hgt_innov.update(innov.rng_vpos, alpha, _hgt_innov_spike_lim);
        has_failed |= checkInnovFailed(rng_hgt_innov_lpf, innov.rng_vpos, _hgt_innov_test_lim, _hgt_innov_spike_lim);
    }

#endif // CONFIG_EKF2_RANGE_FINDER

    if (_is_using_ev_hgt_aiding) {
        const float ev_hgt_innov_lpf = _filter_ev_hgt_innov.update(innov.ev_vpos, alpha, _hgt_innov_spike_lim);
        has_failed |= checkInnovFailed(ev_hgt_innov_lpf, innov.ev_vpos, _hgt_innov_test_lim, _hgt_innov_spike_lim);
    }

    return has_failed;
}

bool PreFlightChecker::checkInnovFailed(const float innov_lpf, const float innov, const float test_limit,
                                        const float spike_limit) {
    return fabsf(innov_lpf) > test_limit || fabsf(innov) > spike_limit;
}

bool PreFlightChecker::checkInnov2DFailed(const Vector2f &innov_lpf, const Vector2f &innov, const float test_limit,
                                          const float spike_limit) {
    return innov_lpf.norm_squared() > sq(test_limit) || innov.norm_squared() > sq(spike_limit);
}

void PreFlightChecker::reset() {
    _is_using_gps_aiding      = false;
    _is_using_ev_pos_aiding   = false;
    _is_using_ev_vel_aiding   = false;
    _is_using_baro_hgt_aiding = false;
    _is_using_gps_hgt_aiding  = false;
    _is_using_ev_hgt_aiding   = false;
    _has_heading_failed       = false;
    _has_horiz_vel_failed     = false;
    _has_vert_vel_failed      = false;
    _has_height_failed        = false;
    _filter_vel_n_innov.reset();
    _filter_vel_e_innov.reset();
    _filter_vel_d_innov.reset();
    _filter_baro_hgt_innov.reset();
    _filter_gps_hgt_innov.reset();
    _filter_ev_hgt_innov.reset();
    _filter_heading_innov.reset();

#if defined(CONFIG_EKF2_RANGE_FINDER)
    _is_using_rng_hgt_aiding = false;
    _filter_rng_hgt_innov.reset();
#endif // CONFIG_EKF2_RANGE_FINDER

#if defined(CONFIG_EKF2_OPTICAL_FLOW)
    _is_using_flow_aiding = false;
    _filter_flow_x_innov.reset();
    _filter_flow_y_innov.reset();
#endif // CONFIG_EKF2_OPTICAL_FLOW
}
