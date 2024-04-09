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
 * @file bias_estimator.hpp
 * @brief Implementation of a single-state bias estimator
 *
 * state: bias
 *
 * The state is noise driven: Transition matrix A = 1
 * x[k+1] = Ax[k] + v with v ~ N(0, Q)
 * y[k] = x[k] + w with w ~ N(0, R)
 *
 * During the measurment update step, the Normalized Innovation Squared (NIS) is checked
 * and the measurement is rejected if larger than the gate size.
 *
 * @author Mathieu Bresciani 	<mathieu@auterion.com>
 */

#pragma once

#include <matrix/math.hpp>
#include <mathlib/mathlib.h>
#include <mathlib/math/filter/AlphaFilter.hpp>

class BiasEstimator {
public:
    struct status {
        float bias{0.f};
        float bias_var{0.f};
        float innov{0.f};
        float innov_var{0.f};
        float innov_test_ratio{INFINITY};
    };

    BiasEstimator() {
    }
    BiasEstimator(float state_init, float state_var_init) :
        _state{state_init}, _state_var{state_var_init} {};

    virtual ~BiasEstimator() = default;

    void reset() {
        _state     = 0.f;
        _state_var = 0.f;
        _signed_innov_test_ratio_lpf.reset(0.f);
        _time_since_last_negative_innov = 0.f;
        _time_since_last_positive_innov = 0.f;
    }

    virtual void predict(float dt);
    virtual void fuseBias(float measurement, float measurement_var);

    void setBias(float bias) {
        _state = bias;
    }
    void setProcessNoiseSpectralDensity(float nsd) {
        _process_psd = nsd * nsd;
    }
    void setBiasStdDev(float state_noise) {
        _state_var = state_noise * state_noise;
    }
    void setInnovGate(float gate_size) {
        _gate_size = gate_size;
    }

    void setMaxStateNoise(float max_noise) {
        _state_var_max = math::max(sq(0.01f), max_noise * max_noise);
    }

    float getBias() const {
        return _state;
    }
    float getBiasVar() const {
        return _state_var;
    }
    const status &getStatus() const {
        return _status;
    }

private:
    float _state{0.f};
    float _dt{0.01f};

    float _gate_size{3.f};        ///< Used for innovation filtering (innovation test ratio)
    float _state_var{0.1f};       ///< Initial state uncertainty variance (m^2)
    float _process_psd{1.25e-6f}; ///< State process power spectral density (m^2/s^2/Hz)
    float _state_var_max{2.f};    ///< Used to constrain the state variance (m^2)

    // Innovation sequence monitoring; used to detect a bias in the state
    AlphaFilter<float> _signed_innov_test_ratio_lpf;
    float              _time_since_last_negative_innov{0.f};
    float              _time_since_last_positive_innov{0.f};

    status _status{};

    void  constrainStateVar();
    float computeKalmanGain(float innov_var) const;

    /*
     * Compute the ratio between the Normalized Innovation Squared (NIS)
     * and its maximum gate size. Use isTestRatioPassing to know if the
     * measurement should be fused or not.
     */
    float computeInnovTestRatio(float innov, float innov_var) const;
    bool  isTestRatioPassing(float innov_test_ratio) const;

    void updateState(float K, float innov);
    void updateStateCovariance(float K);

    void updateOffsetDetection(float innov, float innov_test_ratio);
    bool isOffsetDetected() const;

    status packStatus(float innov, float innov_var, float innov_test_ratio) const;

    static constexpr float _innov_sequence_monitnoring_time_constant{10.f}; ///< in seconds
    static constexpr float _process_var_boost_gain{1.0e3f};
};
