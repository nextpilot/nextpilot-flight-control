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
 * @file PreFlightChecker.hpp
 * Class handling the EKF2 innovation pre flight checks
 *
 * First call the update(...) function and then get the results
 * using the hasXxxFailed() getters
 */

#ifndef EKF2_PREFLIGHTCHECKER_HPP
#define EKF2_PREFLIGHTCHECKER_HPP

#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/estimator_innovations.h>

#include <matrix/matrix/math.hpp>

#include "InnovationLpf.hpp"

using matrix::Vector2f;

class PreFlightChecker {
public:
    PreFlightChecker()  = default;
    ~PreFlightChecker() = default;

    /*
     * Reset all the internal states of the class to their default value
     */
    void reset();

    /*
     * Update the internal states
     * @param dt the sampling time
     * @param innov the ekf2_innovation_s struct containing the current innovations
     */
    void update(float dt, const estimator_innovations_s &innov);

    /*
     * If set to true, the checker will use a less conservative heading innovation check
     */
    void setVehicleCanObserveHeadingInFlight(bool val) {
        _can_observe_heading_in_flight = val;
    }

    void setUsingGpsAiding(bool val) {
        _is_using_gps_aiding = val;
    }
#if defined(CONFIG_EKF2_OPTICAL_FLOW)
    void setUsingFlowAiding(bool val) {
        _is_using_flow_aiding = val;
    }
    void setDistBottom(float dist_bottom) {
        _flow_dist_bottom = dist_bottom;
    }
#endif // CONFIG_EKF2_OPTICAL_FLOW
    void setUsingEvPosAiding(bool val) {
        _is_using_ev_pos_aiding = val;
    }
    void setUsingEvVelAiding(bool val) {
        _is_using_ev_vel_aiding = val;
    }

    void setUsingBaroHgtAiding(bool val) {
        _is_using_baro_hgt_aiding = val;
    }
    void setUsingGpsHgtAiding(bool val) {
        _is_using_gps_hgt_aiding = val;
    }
#if defined(CONFIG_EKF2_RANGE_FINDER)
    void setUsingRngHgtAiding(bool val) {
        _is_using_rng_hgt_aiding = val;
    }
#endif // CONFIG_EKF2_RANGE_FINDER
    void setUsingEvHgtAiding(bool val) {
        _is_using_ev_hgt_aiding = val;
    }

    bool hasHeadingFailed() const {
        return _has_heading_failed;
    }
    bool hasHorizVelFailed() const {
        return _has_horiz_vel_failed;
    }
    bool hasVertVelFailed() const {
        return _has_vert_vel_failed;
    }
    bool hasHeightFailed() const {
        return _has_height_failed;
    }

    /*
     * Overall state of the pre fligh checks
     * @return true if any of the check failed
     */
    bool hasFailed() const {
        return hasHorizFailed() || hasVertFailed();
    }

    /*
     * Horizontal checks overall result
     * @return true if one of the horizontal checks failed
     */
    bool hasHorizFailed() const {
        return _has_heading_failed || _has_horiz_vel_failed;
    }

    /*
     * Vertical checks overall result
     * @return true if one of the vertical checks failed
     */
    bool hasVertFailed() const {
        return _has_vert_vel_failed || _has_height_failed;
    }

    /*
     * Check if the innovation fails the test
     * To pass the test, the following conditions should be true:
     * innov_lpf <= test_limit
     * innov <= spike_limit
     * @param innov_lpf the low-pass filtered innovation
     * @param innov the current unfiltered innovation
     * @param test_limit the magnitude test limit for innov_lpf
     * @param spike_limit the magnitude test limit for innov
     * @return true if the check failed the test, false otherwise
     */
    static bool checkInnovFailed(float innov_lpf, float innov, float test_limit, float spike_limit);

    /*
     * Check if the a innovation of a 2D vector fails the test
     * To pass the test, the following conditions should be true:
     * innov_lpf <= test_limit
     * innov <= spike_limit
     * @param innov_lpf the low-pass filtered innovation
     * @param innov the current unfiltered innovation
     * @param test_limit the magnitude test limit for innov_lpf
     * @param spike_limit the magnitude test limit for innov
     * @return true if the check failed the test, false otherwise
     */
    static bool checkInnov2DFailed(const Vector2f &innov_lpf, const Vector2f &innov, float test_limit, float spike_limit);

    static constexpr float sq(float var) {
        return var * var;
    }

private:
    bool  preFlightCheckHeadingFailed(const estimator_innovations_s &innov, float alpha);
    float selectHeadingTestLimit();

    bool preFlightCheckHorizVelFailed(const estimator_innovations_s &innov, float alpha);
    bool preFlightCheckVertVelFailed(const estimator_innovations_s &innov, float alpha);
    bool preFlightCheckHeightFailed(const estimator_innovations_s &innov, float alpha);

    bool _has_heading_failed{};
    bool _has_horiz_vel_failed{};
    bool _has_vert_vel_failed{};
    bool _has_height_failed{};

    bool _can_observe_heading_in_flight{};
    bool _is_using_gps_aiding{};
    bool _is_using_ev_pos_aiding{};
    bool _is_using_ev_vel_aiding{};

    bool _is_using_baro_hgt_aiding{};
    bool _is_using_gps_hgt_aiding{};
    bool _is_using_ev_hgt_aiding{};

    // Low-pass filters for innovation pre-flight checks
    InnovationLpf _filter_vel_n_innov;   ///< Preflight low pass filter N axis velocity innovations (m/sec)
    InnovationLpf _filter_vel_e_innov;   ///< Preflight low pass filter E axis velocity innovations (m/sec)
    InnovationLpf _filter_vel_d_innov;   ///< Preflight low pass filter D axis velocity innovations (m/sec)
    InnovationLpf _filter_heading_innov; ///< Preflight low pass filter heading innovation magntitude (rad)

    // Preflight low pass filter height innovation (m)
    InnovationLpf _filter_baro_hgt_innov;
    InnovationLpf _filter_gps_hgt_innov;
    InnovationLpf _filter_ev_hgt_innov;

#if defined(CONFIG_EKF2_RANGE_FINDER)
    bool          _is_using_rng_hgt_aiding{};
    InnovationLpf _filter_rng_hgt_innov;
#endif // CONFIG_EKF2_RANGE_FINDER

#if defined(CONFIG_EKF2_OPTICAL_FLOW)
    bool          _is_using_flow_aiding{};
    float         _flow_dist_bottom{};
    InnovationLpf _filter_flow_x_innov; ///< Preflight low pass filter optical flow innovation (rad)
    InnovationLpf _filter_flow_y_innov; ///< Preflight low pass filter optical flow innovation (rad)

    // Maximum permissible flow innovation to pass pre-flight checks
    static constexpr float _flow_innov_test_lim = 0.5f;

    // Preflight flow innovation spike limit (rad)
    static constexpr float _flow_innov_spike_lim = 2.0f * _flow_innov_test_lim;
#endif // CONFIG_EKF2_OPTICAL_FLOW

    // Preflight low pass filter time constant inverse (1/sec)
    static constexpr float _innov_lpf_tau_inv = 0.2f;
    // Maximum permissible velocity innovation to pass pre-flight checks (m/sec)
    static constexpr float _vel_innov_test_lim = 0.5f;
    // Maximum permissible height innovation to pass pre-flight checks (m)
    static constexpr float _hgt_innov_test_lim = 1.5f;
    // Maximum permissible yaw innovation to pass pre-flight checks when aiding inertial nav using NE frame observations (rad)
    static constexpr float _nav_heading_innov_test_lim = 0.25f;
    // Maximum permissible yaw innovation to pass pre-flight checks when not aiding inertial nav using NE frame observations (rad)
    static constexpr float _heading_innov_test_lim = 0.52f;

    // Preflight velocity innovation spike limit (m/sec)
    static constexpr float _vel_innov_spike_lim = 2.0f * _vel_innov_test_lim;
    // Preflight position innovation spike limit (m)
    static constexpr float _hgt_innov_spike_lim = 2.0f * _hgt_innov_test_lim;
};
#endif // !EKF2_PREFLIGHTCHECKER_HPP
