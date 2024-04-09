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
 * @file MulticopterLandDetector.h
 * Land detection implementation for multicopters.
 *
 * @author Johan Jansen <jnsn.johan@gmail.com>
 * @author Morten Lysgaard <morten@lysgaard.no>
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include <uORB/topics/hover_thrust_estimate.h>
#include <uORB/topics/trajectory_setpoint.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>
#include <uORB/topics/takeoff_status.h>

#include "LandDetector.h"

using namespace time_literals;

namespace land_detector {

class MulticopterLandDetector : public LandDetector {
public:
    MulticopterLandDetector();
    ~MulticopterLandDetector() override = default;

protected:
    void _update_params() override;
    void _update_topics() override;

    bool _get_landed_state() override;
    bool _get_ground_contact_state() override;
    bool _get_maybe_landed_state() override;
    bool _get_freefall_state() override;
    bool _get_ground_effect_state() override;
    bool _get_in_descend() override {
        return _in_descend;
    }
    bool _get_has_low_throttle() override {
        return _has_low_throttle;
    }
    bool _get_horizontal_movement() override {
        return _horizontal_movement;
    }
    bool _get_vertical_movement() override {
        return _vertical_movement;
    }
    bool _get_rotational_movement() override {
        return _rotational_movement;
    }
    bool _get_close_to_ground_or_skipped_check() override {
        return _close_to_ground_or_skipped_check;
    }

    void _set_hysteresis_factor(const int factor) override;

private:
    bool _is_close_to_ground();

    /** Time in us that freefall has to hold before triggering freefall */
    static constexpr hrt_abstime FREEFALL_TRIGGER_TIME_US = 300_ms;

    /** Distance above ground below which entering ground contact state is possible when distance to ground is available. */
    static constexpr float DIST_FROM_GROUND_THRESHOLD = 1.0f;

    struct {
        param_t minThrottle;
        param_t hoverThrottle;
        param_t minManThrottle;
        param_t useHoverThrustEstimate;
        param_t landSpeed;
        param_t crawlSpeed;
    } _paramHandle{};

    struct {
        float minThrottle;
        float hoverThrottle;
        float minManThrottle;
        bool  useHoverThrustEstimate;
        float landSpeed;
        float crawlSpeed;
    } _params{};

    uORB::Subscription _vehicle_thrust_setpoint_sub{ORB_ID(vehicle_thrust_setpoint)};
    uORB::Subscription _hover_thrust_estimate_sub{ORB_ID(hover_thrust_estimate)};
    uORB::Subscription _trajectory_setpoint_sub{ORB_ID(trajectory_setpoint)};

    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::Subscription _takeoff_status_sub{ORB_ID(takeoff_status)};

    hrt_abstime _hover_thrust_estimate_last_valid{0};
    bool        _hover_thrust_estimate_valid{false};

    bool _flag_control_climb_rate_enabled{false};
    bool _hover_thrust_initialized{false};

    float _vehicle_thrust_setpoint_throttle{0.f};

    uint8_t _takeoff_state{takeoff_status_s::TAKEOFF_STATE_DISARMED};

    systemlib::Hysteresis _minimum_thrust_8s_hysteresis{false};

    bool _in_descend{false};          ///< vehicle is commanded to desend
    bool _horizontal_movement{false}; ///< vehicle is moving horizontally
    bool _vertical_movement{false};
    bool _rotational_movement{false};
    bool _has_low_throttle{false};
    bool _close_to_ground_or_skipped_check{false};
    bool _below_gnd_effect_hgt{false}; ///< vehicle height above ground is below height where ground effect occurs

    DEFINE_PARAMETERS_CUSTOM_PARENT(
        LandDetector,
        (ParamFloat<px4::params::LNDMC_TRIG_TIME>)_param_lndmc_trig_time,
        (ParamFloat<px4::params::LNDMC_ROT_MAX>)_param_lndmc_rot_max,
        (ParamFloat<px4::params::LNDMC_XY_VEL_MAX>)_param_lndmc_xy_vel_max,
        (ParamFloat<px4::params::LNDMC_Z_VEL_MAX>)_param_lndmc_z_vel_max,
        (ParamFloat<px4::params::LNDMC_ALT_GND>)_param_lndmc_alt_gnd_effect);
};

} // namespace land_detector
