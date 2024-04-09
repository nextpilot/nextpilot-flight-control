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
 * @file FixedwingLandDetector.h
 * Land detector implementation for fixedwing.
 *
 * @author Johan Jansen <jnsn.johan@gmail.com>
 * @author Morten Lysgaard <morten@lysgaard.no>
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include <matrix/math.hpp>
#include <uORB/topics/airspeed_validated.h>
#include <uORB/topics/launch_detection_status.h>

#include "LandDetector.h"

using namespace time_literals;

namespace land_detector {

class FixedwingLandDetector final : public LandDetector {
public:
    FixedwingLandDetector();
    ~FixedwingLandDetector() override = default;

protected:
    bool _get_landed_state() override;
    void _set_hysteresis_factor(const int factor) override{};

private:
    static constexpr hrt_abstime FLYING_TRIGGER_TIME_US = 0_us;

    uORB::Subscription _airspeed_validated_sub{ORB_ID(airspeed_validated)};
    uORB::Subscription _launch_detection_status_sub{ORB_ID(launch_detection_status)};

    float _airspeed_filtered{0.0f};
    float _velocity_xy_filtered{0.0f};
    float _velocity_z_filtered{0.0f};
    float _xy_accel_filtered{0.0f};

    DEFINE_PARAMETERS_CUSTOM_PARENT(
        LandDetector,
        (ParamFloat<params_id::LNDFW_XYACC_MAX>)_param_lndfw_xyaccel_max,
        (ParamFloat<params_id::LNDFW_AIRSPD_MAX>)_param_lndfw_airspd,
        (ParamFloat<params_id::LNDFW_VEL_XY_MAX>)_param_lndfw_vel_xy_max,
        (ParamFloat<params_id::LNDFW_VEL_Z_MAX>)_param_lndfw_vel_z_max,
        (ParamFloat<params_id::LNDFW_TRIG_TIME>)_param_lndfw_trig_time);
};

} // namespace land_detector
