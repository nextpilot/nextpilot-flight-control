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
 * @file bias_estimator.cpp
 *
 * @author Mathieu Bresciani 	<mathieu@auterion.com>
 */

#include "bias_estimator.hpp"

void BiasEstimator::predict(const float dt) {
    // State is constant
    // Predict state covariance only
    float delta_state_var = _process_psd * dt;

    if (isOffsetDetected()) {
        // A bias in the state has been detected by the innovation sequence check
        // Boost the process noise until the offset is removed
        delta_state_var *= _process_var_boost_gain;
    }

    _state_var += delta_state_var;
    constrainStateVar();

    if (dt > FLT_EPSILON && fabsf(_dt - dt) > 0.001f) {
        _signed_innov_test_ratio_lpf.setParameters(dt, _innov_sequence_monitnoring_time_constant);
        _dt = dt;
    }

    _status.bias_var = _state_var;
}

void BiasEstimator::constrainStateVar() {
    _state_var = math::constrain(_state_var, 1e-8f, _state_var_max);
}

void BiasEstimator::fuseBias(const float measurement, const float measurement_var) {
    const float innov_var        = _state_var + math::max(sq(0.01f), measurement_var);
    const float innov            = measurement - _state;
    const float K                = _state_var / innov_var;
    const float innov_test_ratio = computeInnovTestRatio(innov, innov_var);

    if (isTestRatioPassing(innov_test_ratio)) {
        updateState(K, innov);
        updateStateCovariance(K);
    }

    updateOffsetDetection(innov, innov_test_ratio);

    _status = packStatus(innov, innov_var, innov_test_ratio);
}

inline float BiasEstimator::computeInnovTestRatio(const float innov, const float innov_var) const {
    return innov * innov / (_gate_size * _gate_size * innov_var);
}

inline bool BiasEstimator::isTestRatioPassing(const float innov_test_ratio) const {
    return innov_test_ratio < 1.f;
}

inline void BiasEstimator::updateState(const float K, const float innov) {
    _state = _state + K * innov;
}

inline void BiasEstimator::updateStateCovariance(const float K) {
    _state_var -= K * _state_var;
    constrainStateVar();
}

inline void BiasEstimator::updateOffsetDetection(const float innov, const float innov_test_ratio) {
    const float signed_innov_test_ratio = matrix::sign(innov) * innov_test_ratio;
    _signed_innov_test_ratio_lpf.update(math::constrain(signed_innov_test_ratio, -1.f, 1.f));

    if (innov > 0.f) {
        _time_since_last_positive_innov = 0.f;
        _time_since_last_negative_innov += _dt;

    } else {
        _time_since_last_negative_innov = 0.f;
        _time_since_last_positive_innov += _dt;
    }
}

inline bool BiasEstimator::isOffsetDetected() const {
    // There is an offset in the estimate if the average of innovation is statistically too large
    // or if the sign of the innovation is constantly the same
    return fabsf(_signed_innov_test_ratio_lpf.getState()) > 0.2f || (_time_since_last_positive_innov > _innov_sequence_monitnoring_time_constant) || (_time_since_last_negative_innov > _innov_sequence_monitnoring_time_constant);
}

inline BiasEstimator::status BiasEstimator::packStatus(const float innov, const float innov_var,
                                                       const float innov_test_ratio) const {
    // Send back status for logging
    status ret{};
    ret.bias             = _state;
    ret.bias_var         = _state_var;
    ret.innov            = innov;
    ret.innov_var        = innov_var;
    ret.innov_test_ratio = innov_test_ratio;

    return ret;
}
