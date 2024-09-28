/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <matrix/matrix/math.hpp>
#include <perf/perf_counter.h>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <ulog/mavlink_log.h>
#include <workq/WorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>

#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/autotune_attitude_control_status.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_rates_setpoint.h>
#include <uORB/topics/vehicle_status.h>
#include <mathlib/math/filter/AlphaFilter.hpp>
#include "AttitudeControl/AttitudeControl.hpp"

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::workq;
using namespace nextpilot::param;

class MulticopterAttitudeControl : public ModuleCommand<MulticopterAttitudeControl>, public ModuleParams, public WorkItem {
public:
    MulticopterAttitudeControl(bool vtol = false);
    ~MulticopterAttitudeControl() override;

    /** @see ModuleCommand */
    static MulticopterAttitudeControl *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;

    /**
     * initialize some vectors/matrices from parameters
     */
    void parameters_updated();

    float throttle_curve(float throttle_stick_input);

    /**
     * Generate & publish an attitude setpoint from stick inputs
     */
    void generate_attitude_setpoint(const matrix::Quatf &q, float dt, bool reset_yaw_sp);

    AttitudeControl _attitude_control; /**< class for attitude control calculations */

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _autotune_attitude_control_status_sub{ORB_ID(autotune_attitude_control_status)};
    uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};
    uORB::Subscription _vehicle_attitude_setpoint_sub{ORB_ID(vehicle_attitude_setpoint)};
    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::Subscription _vehicle_local_position_sub{ORB_ID(vehicle_local_position)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::SubscriptionCallbackWorkItem _vehicle_attitude_sub{this, ORB_ID(vehicle_attitude)};

    uORB::Publication<vehicle_rates_setpoint_s>    _vehicle_rates_setpoint_pub{ORB_ID(vehicle_rates_setpoint)}; /**< rate setpoint publication */
    uORB::Publication<vehicle_attitude_setpoint_s> _vehicle_attitude_setpoint_pub;

    manual_control_setpoint_s _manual_control_setpoint{}; /**< manual control setpoint */
    vehicle_control_mode_s    _vehicle_control_mode{};    /**< vehicle control mode */

    perf_counter_t _loop_perf;                            /**< loop duration performance counter */

    matrix::Vector3f _thrust_setpoint_body;               /**< body frame 3D thrust vector */

    float _man_yaw_sp{0.f};                               /**< current yaw setpoint in manual mode */
    float _man_tilt_max;                                  /**< maximum tilt allowed for manual flight [rad] */

    AlphaFilter<float> _man_roll_input_filter;
    AlphaFilter<float> _man_pitch_input_filter;

    hrt_abstime _last_run{0};
    hrt_abstime _last_attitude_setpoint{0};

    bool _reset_yaw_sp{true};
    bool _heading_good_for_control{true}; ///< initialized true to have heading lock when local position never published
    bool _vehicle_type_rotary_wing{true};
    bool _vtol{false};
    bool _vtol_tailsitter{false};
    bool _vtol_in_transition_mode{false};

    uint8_t _quat_reset_counter{0};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::MC_AIRMODE>)_param_mc_airmode,
        (ParamFloat<params_id::MC_MAN_TILT_TAU>)_param_mc_man_tilt_tau,

        (ParamFloat<params_id::MC_ROLL_P>)_param_mc_roll_p,
        (ParamFloat<params_id::MC_PITCH_P>)_param_mc_pitch_p,
        (ParamFloat<params_id::MC_YAW_P>)_param_mc_yaw_p,
        (ParamFloat<params_id::MC_YAW_WEIGHT>)_param_mc_yaw_weight,

        (ParamFloat<params_id::MC_ROLLRATE_MAX>)_param_mc_rollrate_max,
        (ParamFloat<params_id::MC_PITCHRATE_MAX>)_param_mc_pitchrate_max,
        (ParamFloat<params_id::MC_YAWRATE_MAX>)_param_mc_yawrate_max,

        /* Stabilized mode params */
        (ParamFloat<params_id::MPC_MAN_TILT_MAX>)_param_mpc_man_tilt_max, /**< maximum tilt allowed for manual flight */
        (ParamFloat<params_id::MPC_MAN_Y_MAX>)_param_mpc_man_y_max,       /**< scaling factor from stick to yaw rate */
        (ParamFloat<params_id::MPC_MANTHR_MIN>)_param_mpc_manthr_min,     /**< minimum throttle for stabilized */
        (ParamFloat<params_id::MPC_THR_MAX>)_param_mpc_thr_max,           /**< maximum throttle for stabilized */
        (ParamFloat<params_id::MPC_THR_HOVER>)_param_mpc_thr_hover,       /**< throttle at stationary hover */
        (ParamInt<params_id::MPC_THR_CURVE>)_param_mpc_thr_curve          /**< throttle curve behavior */
    )
};
