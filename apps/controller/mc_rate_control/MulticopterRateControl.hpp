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

#include <rate_control/rate_control.hpp>
#include <matrix/matrix/math.hpp>
#include <perf/perf_counter.h>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <ulog/mavlink_log.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/actuator_controls_status.h>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/control_allocator_status.h>
#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/rate_ctrl_status.h>
#include <uORB/topics/vehicle_angular_velocity.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_land_detected.h>
#include <uORB/topics/vehicle_rates_setpoint.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>
#include <uORB/topics/vehicle_torque_setpoint.h>

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::workq;
using namespace nextpilot::param;

class MulticopterRateControl : public ModuleCommand<MulticopterRateControl>, public ModuleParams, public WorkItem {
public:
    MulticopterRateControl(bool vtol = false);
    ~MulticopterRateControl() override;

    /** @see ModuleCommand */
    static MulticopterRateControl *instantiate(int argc, char *argv[]);

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

    void updateActuatorControlsStatus(const vehicle_torque_setpoint_s &vehicle_torque_setpoint, float dt);

    RateControl _rate_control; ///< class for rate control calculations

    uORB::Subscription _battery_status_sub{ORB_ID(battery_status)};
    uORB::Subscription _control_allocator_status_sub{ORB_ID(control_allocator_status)};
    uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};
    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::Subscription _vehicle_land_detected_sub{ORB_ID(vehicle_land_detected)};
    uORB::Subscription _vehicle_rates_setpoint_sub{ORB_ID(vehicle_rates_setpoint)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::SubscriptionCallbackWorkItem _vehicle_angular_velocity_sub{this, ORB_ID(vehicle_angular_velocity)};

    uORB::Publication<actuator_controls_status_s> _actuator_controls_status_pub{ORB_ID(actuator_controls_status_0)};
    uORB::PublicationMulti<rate_ctrl_status_s>    _controller_status_pub{ORB_ID(rate_ctrl_status)};
    uORB::Publication<vehicle_rates_setpoint_s>   _vehicle_rates_setpoint_pub{ORB_ID(vehicle_rates_setpoint)};
    uORB::Publication<vehicle_torque_setpoint_s>  _vehicle_torque_setpoint_pub;
    uORB::Publication<vehicle_thrust_setpoint_s>  _vehicle_thrust_setpoint_pub;

    vehicle_control_mode_s _vehicle_control_mode{};
    vehicle_status_s       _vehicle_status{};

    bool _landed{true};
    bool _maybe_landed{true};

    hrt_abstime _last_run{0};

    perf_counter_t _loop_perf; /**< loop duration performance counter */

    // keep setpoint values between updates
    matrix::Vector3f _acro_rate_max; /**< max attitude rates in acro mode */
    matrix::Vector3f _rates_setpoint{};

    float            _battery_status_scale{0.0f};
    matrix::Vector3f _thrust_setpoint{};

    float _energy_integration_time{0.0f};
    float _control_energy[4]{};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::MC_ROLLRATE_P>)_param_mc_rollrate_p,
        (ParamFloat<params_id::MC_ROLLRATE_I>)_param_mc_rollrate_i,
        (ParamFloat<params_id::MC_RR_INT_LIM>)_param_mc_rr_int_lim,
        (ParamFloat<params_id::MC_ROLLRATE_D>)_param_mc_rollrate_d,
        (ParamFloat<params_id::MC_ROLLRATE_FF>)_param_mc_rollrate_ff,
        (ParamFloat<params_id::MC_ROLLRATE_K>)_param_mc_rollrate_k,

        (ParamFloat<params_id::MC_PITCHRATE_P>)_param_mc_pitchrate_p,
        (ParamFloat<params_id::MC_PITCHRATE_I>)_param_mc_pitchrate_i,
        (ParamFloat<params_id::MC_PR_INT_LIM>)_param_mc_pr_int_lim,
        (ParamFloat<params_id::MC_PITCHRATE_D>)_param_mc_pitchrate_d,
        (ParamFloat<params_id::MC_PITCHRATE_FF>)_param_mc_pitchrate_ff,
        (ParamFloat<params_id::MC_PITCHRATE_K>)_param_mc_pitchrate_k,

        (ParamFloat<params_id::MC_YAWRATE_P>)_param_mc_yawrate_p,
        (ParamFloat<params_id::MC_YAWRATE_I>)_param_mc_yawrate_i,
        (ParamFloat<params_id::MC_YR_INT_LIM>)_param_mc_yr_int_lim,
        (ParamFloat<params_id::MC_YAWRATE_D>)_param_mc_yawrate_d,
        (ParamFloat<params_id::MC_YAWRATE_FF>)_param_mc_yawrate_ff,
        (ParamFloat<params_id::MC_YAWRATE_K>)_param_mc_yawrate_k,

        (ParamFloat<params_id::MC_ACRO_R_MAX>)_param_mc_acro_r_max,
        (ParamFloat<params_id::MC_ACRO_P_MAX>)_param_mc_acro_p_max,
        (ParamFloat<params_id::MC_ACRO_Y_MAX>)_param_mc_acro_y_max,
        (ParamFloat<params_id::MC_ACRO_EXPO>)_param_mc_acro_expo,         /**< expo stick curve shape (roll & pitch) */
        (ParamFloat<params_id::MC_ACRO_EXPO_Y>)_param_mc_acro_expo_y,     /**< expo stick curve shape (yaw) */
        (ParamFloat<params_id::MC_ACRO_SUPEXPO>)_param_mc_acro_supexpo,   /**< superexpo stick curve shape (roll & pitch) */
        (ParamFloat<params_id::MC_ACRO_SUPEXPOY>)_param_mc_acro_supexpoy, /**< superexpo stick curve shape (yaw) */

        (ParamBool<params_id::MC_BAT_SCALE_EN>)_param_mc_bat_scale_en)
};
