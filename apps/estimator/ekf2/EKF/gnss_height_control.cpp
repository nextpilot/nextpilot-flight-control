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
 * @file gnss_height_control.cpp
 * Control functions for ekf GNSS height fusion
 */

#include "ekf.h"

void Ekf::controlGnssHeightFusion(const gpsSample &gps_sample) {
    static constexpr const char *HGT_SRC_NAME = "GNSS";

    auto                &aid_src  = _aid_src_gnss_hgt;
    HeightBiasEstimator &bias_est = _gps_hgt_b_est;

    bias_est.predict(_dt_ekf_avg);

    if (_gps_data_ready) {
        // relax the upper observation noise limit which prevents bad GPS perturbing the position estimate
        float noise = math::max(gps_sample.vacc, 1.5f * _params.gps_pos_noise); // use 1.5 as a typical ratio of vacc/hacc

        if (!isOnlyActiveSourceOfVerticalPositionAiding(_control_status.flags.gps_hgt)) {
            // if we are not using another source of aiding, then we are reliant on the GPS
            // observations to constrain attitude errors and must limit the observation noise value.
            if (noise > _params.pos_noaid_noise) {
                noise = _params.pos_noaid_noise;
            }
        }

        const float measurement     = gps_sample.hgt - getEkfGlobalOriginAltitude();
        const float measurement_var = sq(noise);

        const float innov_gate = math::max(_params.gps_pos_innov_gate, 1.f);

        const bool measurement_valid = PX4_ISFINITE(measurement) && PX4_ISFINITE(measurement_var);

        // GNSS position, vertical position GNSS measurement has opposite sign to earth z axis
        updateVerticalPositionAidSrcStatus(gps_sample.time_us,
                                           -(measurement - bias_est.getBias()),
                                           measurement_var + bias_est.getBiasVar(),
                                           innov_gate,
                                           aid_src);

        const bool gps_checks_passing = isTimedOut(_last_gps_fail_us, (uint64_t)5e6);
        const bool gps_checks_failing = isTimedOut(_last_gps_pass_us, (uint64_t)5e6);

        // update the bias estimator before updating the main filter but after
        // using its current state to compute the vertical position innovation
        if (measurement_valid && gps_checks_passing && !gps_checks_failing) {
            bias_est.setMaxStateNoise(sqrtf(measurement_var));
            bias_est.setProcessNoiseSpectralDensity(_params.gps_hgt_bias_nsd);
            bias_est.fuseBias(measurement - (-_state.pos(2)), measurement_var + P(9, 9));
        }

        // determine if we should use height aiding
        const bool continuing_conditions_passing = (_params.gnss_ctrl & GnssCtrl::VPOS) && measurement_valid && _NED_origin_initialised && _gps_checks_passed;

        const bool starting_conditions_passing = continuing_conditions_passing && isNewestSampleRecent(_time_last_gps_buffer_push, 2 * GNSS_MAX_INTERVAL) && _gps_checks_passed && gps_checks_passing && !gps_checks_failing;

        if (_control_status.flags.gps_hgt) {
            aid_src.fusion_enabled = true;

            if (continuing_conditions_passing) {
                fuseVerticalPosition(aid_src);

                const bool is_fusion_failing = isTimedOut(aid_src.time_last_fuse, _params.hgt_fusion_timeout_max);

                if (isHeightResetRequired()) {
                    // All height sources are failing
                    ECL_WARN("%s height fusion reset required, all height sources failing", HGT_SRC_NAME);

                    _information_events.flags.reset_hgt_to_gps = true;
                    resetVerticalPositionTo(-(measurement - bias_est.getBias()), measurement_var);
                    bias_est.setBias(_state.pos(2) + measurement);

                    // reset vertical velocity
                    if (PX4_ISFINITE(gps_sample.vel(2)) && (_params.gnss_ctrl & GnssCtrl::VEL)) {
                        // use 1.5 as a typical ratio of vacc/hacc
                        resetVerticalVelocityTo(gps_sample.vel(2), sq(math::max(1.5f * gps_sample.sacc, _params.gps_vel_noise)));

                    } else {
                        resetVerticalVelocityToZero();
                    }

                    aid_src.time_last_fuse = _time_delayed_us;

                } else if (is_fusion_failing) {
                    // Some other height source is still working
                    ECL_WARN("stopping %s height fusion, fusion failing", HGT_SRC_NAME);
                    stopGpsHgtFusion();
                }

            } else {
                ECL_WARN("stopping %s height fusion, continuing conditions failing", HGT_SRC_NAME);
                stopGpsHgtFusion();
            }

        } else {
            if (starting_conditions_passing) {
                if (_params.height_sensor_ref == HeightSensor::GNSS) {
                    ECL_INFO("starting %s height fusion, resetting height", HGT_SRC_NAME);
                    _height_sensor_ref = HeightSensor::GNSS;

                    _information_events.flags.reset_hgt_to_gps = true;
                    resetVerticalPositionTo(-measurement, measurement_var);
                    bias_est.reset();

                } else {
                    ECL_INFO("starting %s height fusion", HGT_SRC_NAME);
                    bias_est.setBias(_state.pos(2) + measurement);
                }

                aid_src.time_last_fuse = _time_delayed_us;
                bias_est.setFusionActive();
                _control_status.flags.gps_hgt = true;
            }
        }

    } else if (_control_status.flags.gps_hgt && !isNewestSampleRecent(_time_last_gps_buffer_push, 2 * GNSS_MAX_INTERVAL)) {
        // No data anymore. Stop until it comes back.
        ECL_WARN("stopping %s height fusion, no data", HGT_SRC_NAME);
        stopGpsHgtFusion();
    }
}

void Ekf::stopGpsHgtFusion() {
    if (_control_status.flags.gps_hgt) {
        if (_height_sensor_ref == HeightSensor::GNSS) {
            _height_sensor_ref = HeightSensor::UNKNOWN;
        }

        _gps_hgt_b_est.setFusionInactive();
        resetEstimatorAidStatus(_aid_src_gnss_hgt);

        _control_status.flags.gps_hgt = false;
    }
}
