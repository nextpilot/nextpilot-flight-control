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
 * @file position_bias_estimator.hpp
 */

#pragma once

#include "bias_estimator.hpp"

class PositionBiasEstimator {
public:
    PositionBiasEstimator(uint8_t sensor, const uint8_t &sensor_ref) :
        _sensor(sensor),
        _sensor_ref(sensor_ref) {
    }
    virtual ~PositionBiasEstimator() = default;

    bool fusionActive() const {
        return _is_sensor_fusion_active;
    }

    void setFusionActive() {
        _is_sensor_fusion_active = true;
    }
    void setFusionInactive() {
        _is_sensor_fusion_active = false;
    }

    void predict(float dt) {
        if ((_sensor_ref != _sensor) && _is_sensor_fusion_active) {
            _bias[0].predict(dt);
            _bias[1].predict(dt);
        }
    }

    void fuseBias(Vector2f bias, Vector2f bias_var) {
        if ((_sensor_ref != _sensor) && _is_sensor_fusion_active) {
            _bias[0].fuseBias(bias(0), bias_var(0));
            _bias[1].fuseBias(bias(1), bias_var(1));
        }
    }

    void setBias(const Vector2f &bias) {
        _bias[0].setBias(bias(0));
        _bias[1].setBias(bias(1));
    }

    void setProcessNoiseSpectralDensity(float nsd) {
        _bias[0].setProcessNoiseSpectralDensity(nsd);
        _bias[1].setProcessNoiseSpectralDensity(nsd);
    }
    // void setBiasStdDev(float state_noise) { _state_var = state_noise * state_noise; }
    // void setInnovGate(float gate_size) { _gate_size = gate_size; }

    void setMaxStateNoise(const Vector2f &max_noise) {
        _bias[0].setMaxStateNoise(max_noise(0));
        _bias[1].setMaxStateNoise(max_noise(1));
    }

    Vector2f getBias() const {
        return Vector2f(_bias[0].getBias(), _bias[1].getBias());
    }
    float getBias(int index) const {
        return _bias[index].getBias();
    }

    Vector2f getBiasVar() const {
        return Vector2f(_bias[0].getBiasVar(), _bias[1].getBiasVar());
    }
    float getBiasVar(int index) const {
        return _bias[index].getBiasVar();
    }

    const BiasEstimator::status &getStatus(int index) const {
        return _bias[index].getStatus();
    }

    void reset() {
        _bias[0].reset();
        _bias[1].reset();
    }

private:
    BiasEstimator _bias[2]{};

    const uint8_t  _sensor;
    const uint8_t &_sensor_ref;

    bool _is_sensor_fusion_active{false}; // TODO: replace by const ref and remove setter when migrating _control_status.flags from union to bool
};
