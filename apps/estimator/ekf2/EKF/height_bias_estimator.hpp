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
 * @file height_bias_estimator.hpp
 */

#pragma once

#include "bias_estimator.hpp"

class HeightBiasEstimator : public BiasEstimator {
public:
    HeightBiasEstimator(uint8_t sensor, const uint8_t &sensor_ref) :
        BiasEstimator(0.f, 0.f),
        _sensor(sensor),
        _sensor_ref(sensor_ref) {
    }
    virtual ~HeightBiasEstimator() = default;

    void setFusionActive() {
        _is_sensor_fusion_active = true;
    }
    void setFusionInactive() {
        _is_sensor_fusion_active = false;
    }

    virtual void predict(float dt) override {
        if ((_sensor_ref != _sensor) && _is_sensor_fusion_active) {
            BiasEstimator::predict(dt);
        }
    }

    virtual void fuseBias(float bias, float bias_var) override {
        if ((_sensor_ref != _sensor) && _is_sensor_fusion_active) {
            BiasEstimator::fuseBias(bias, bias_var);
        }
    }

private:
    const uint8_t  _sensor;
    const uint8_t &_sensor_ref;

    bool _is_sensor_fusion_active{false}; // TODO: replace by const ref and remove setter when migrating _control_status.flags from union to bool
};
