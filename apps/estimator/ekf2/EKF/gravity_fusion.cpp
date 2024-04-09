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
 * @file gravity_fusion.cpp
 * Fuse observations from the gravity vector to constrain roll
 * and pitch (a la complementary filter).
 *
 * @author Daniel M. Sahu <danielmohansahu@gmail.com>
 */

#include "ekf.h"
#include "python/ekf_derivation/generated/compute_gravity_innov_var_and_k_and_h.h"

#include <mathlib/mathlib.h>

void Ekf::controlGravityFusion(const imuSample &imu) {
    // fuse gravity observation if our overall acceleration isn't too big
    const float gravity_scale = _accel_vec_filt.norm() / CONSTANTS_ONE_G;

    _control_status.flags.gravity_vector = (_params.imu_ctrl & static_cast<int32_t>(ImuCtrl::GravityVector)) && (((gravity_scale >= 0.9f && gravity_scale <= 1.1f)) || _control_status.flags.vehicle_at_rest) && !isHorizontalAidingActive();

    // get raw accelerometer reading at delayed horizon and expected measurement noise (gaussian)
    const Vector3f measurement     = imu.delta_vel / imu.delta_vel_dt - getAccelBias();
    const float    measurement_var = sq(_params.gravity_noise);

    // calculate kalman gains and innovation variances
    Vector3f  innovation; // innovation of the last gravity fusion observation (m/s**2)
    Vector3f  innovation_variance;
    Vector24f Kx, Ky, Kz; // Kalman gain vectors
    sym::ComputeGravityInnovVarAndKAndH(
        getStateAtFusionHorizonAsVector(), P, measurement, measurement_var, FLT_EPSILON,
        &innovation, &innovation_variance, &Kx, &Ky, &Kz);

    // fill estimator aid source status
    resetEstimatorAidStatus(_aid_src_gravity);
    _aid_src_gravity.timestamp_sample = imu.time_us;
    measurement.copyTo(_aid_src_gravity.observation);

    for (auto &var : _aid_src_gravity.observation_variance) {
        var = measurement_var;
    }

    innovation.copyTo(_aid_src_gravity.innovation);
    innovation_variance.copyTo(_aid_src_gravity.innovation_variance);

    float innovation_gate = 1.f;
    setEstimatorAidStatusTestRatio(_aid_src_gravity, innovation_gate);

    _aid_src_gravity.fusion_enabled = _control_status.flags.gravity_vector;

    if (_aid_src_gravity.fusion_enabled && !_aid_src_gravity.innovation_rejected) {
        // perform fusion for each axis
        _aid_src_gravity.fused = measurementUpdate(Kx, innovation_variance(0), innovation(0)) && measurementUpdate(Ky, innovation_variance(1), innovation(1)) && measurementUpdate(Kz, innovation_variance(2), innovation(2));

        if (_aid_src_gravity.fused) {
            _aid_src_gravity.time_last_fuse = imu.time_us;
        }
    }
}
