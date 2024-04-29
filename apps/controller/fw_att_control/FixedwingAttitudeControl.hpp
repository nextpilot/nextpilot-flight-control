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

#include "nextpilot.h"
// #include <hrtimer.h>
#include "ecl_pitch_controller.h"
#include "ecl_roll_controller.h"
#include "ecl_wheel_controller.h"
#include "ecl_yaw_controller.h"
// #include <mathlib/mathlib.h>
// #include <param/param.h>
// #include <perf/perf_counter.h>
// #include <matrix/math.hpp>

// #include <defines.h>
// #include <module/module_command.hpp>
// #include <module/module_params.hpp>


// #include <workq/WorkItemScheduled.hpp>
// #include <uORB/uORBPublication.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/topics/airspeed_validated.h>
// #include <uORB/topics/autotune_attitude_control_status.h>
// #include <uORB/topics/landing_gear_wheel.h>
// #include <uORB/topics/manual_control_setpoint.h>
// #include <uORB/topics/parameter_update.h>
// #include <uORB/topics/vehicle_angular_velocity.h>
// #include <uORB/topics/vehicle_attitude.h>
// #include <uORB/topics/vehicle_attitude_setpoint.h>
// #include <uORB/topics/vehicle_control_mode.h>
// #include <uORB/topics/vehicle_local_position.h>
// #include <uORB/topics/vehicle_land_detected.h>
// #include <uORB/topics/vehicle_rates_setpoint.h>
// #include <uORB/topics/vehicle_status.h>

using matrix::Eulerf;
using matrix::Quatf;

using uORB::SubscriptionData;

using namespace time_literals;

class FixedwingAttitudeControl final : public ModuleCommand<FixedwingAttitudeControl>, public ModuleParams, public WorkItemScheduled {
public:
    FixedwingAttitudeControl(bool vtol = false);
    ~FixedwingAttitudeControl() override;

    /** @see ModuleCommand */
    static FixedwingAttitudeControl *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;

    uORB::SubscriptionCallbackWorkItem _att_sub{this, ORB_ID(vehicle_attitude)}; /**< vehicle attitude */

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _att_sp_sub{ORB_ID(vehicle_attitude_setpoint)};                /**< vehicle attitude setpoint */
    uORB::Subscription _autotune_attitude_control_status_sub{ORB_ID(autotune_attitude_control_status)};
    uORB::Subscription _local_pos_sub{ORB_ID(vehicle_local_position)};                /**< local position subscription */
    uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)}; /**< notification of manual control updates */
    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};       /**< vehicle status subscription */
    uORB::Subscription _vehicle_land_detected_sub{ORB_ID(vehicle_land_detected)};     /**< vehicle land detected subscription */
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};                   /**< vehicle status subscription */
    uORB::Subscription _vehicle_rates_sub{ORB_ID(vehicle_angular_velocity)};

    uORB::SubscriptionData<airspeed_validated_s> _airspeed_validated_sub{ORB_ID(airspeed_validated)};

    uORB::Publication<vehicle_attitude_setpoint_s> _attitude_sp_pub;
    uORB::Publication<vehicle_rates_setpoint_s>    _rate_sp_pub{ORB_ID(vehicle_rates_setpoint)};
    uORB::Publication<landing_gear_wheel_s>        _landing_gear_wheel_pub{ORB_ID(landing_gear_wheel)};

    manual_control_setpoint_s   _manual_control_setpoint{};
    vehicle_attitude_setpoint_s _att_sp{};
    vehicle_control_mode_s      _vcontrol_mode{};
    vehicle_rates_setpoint_s    _rates_sp{};
    vehicle_status_s            _vehicle_status{};
    landing_gear_wheel_s        _landing_gear_wheel{};

    matrix::Dcmf _R{matrix::eye<float, 3>()};

    perf_counter_t _loop_perf;

    hrt_abstime _last_run{0};

    bool  _landed{true};
    float _groundspeed{0.f};
    bool  _in_fw_or_transition_wo_tailsitter_transition{false}; // only run the FW attitude controller in these states

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::FW_AIRSPD_MAX>)_param_fw_airspd_max,
        (ParamFloat<params_id::FW_AIRSPD_MIN>)_param_fw_airspd_min,
        (ParamFloat<params_id::FW_AIRSPD_STALL>)_param_fw_airspd_stall,
        (ParamFloat<params_id::FW_AIRSPD_TRIM>)_param_fw_airspd_trim,
        (ParamInt<params_id::FW_ARSP_MODE>)_param_fw_arsp_mode,

        (ParamFloat<params_id::FW_MAN_P_MAX>)_param_fw_man_p_max,
        (ParamFloat<params_id::FW_MAN_R_MAX>)_param_fw_man_r_max,

        (ParamFloat<params_id::FW_P_RMAX_NEG>)_param_fw_p_rmax_neg,
        (ParamFloat<params_id::FW_P_RMAX_POS>)_param_fw_p_rmax_pos,
        (ParamFloat<params_id::FW_P_TC>)_param_fw_p_tc,
        (ParamFloat<params_id::FW_PSP_OFF>)_param_fw_psp_off,

        (ParamFloat<params_id::FW_R_RMAX>)_param_fw_r_rmax,
        (ParamFloat<params_id::FW_R_TC>)_param_fw_r_tc,

        (ParamBool<params_id::FW_W_EN>)_param_fw_w_en,
        (ParamFloat<params_id::FW_W_RMAX>)_param_fw_w_rmax,
        (ParamFloat<params_id::FW_WR_FF>)_param_fw_wr_ff,
        (ParamFloat<params_id::FW_WR_I>)_param_fw_wr_i,
        (ParamFloat<params_id::FW_WR_IMAX>)_param_fw_wr_imax,
        (ParamFloat<params_id::FW_WR_P>)_param_fw_wr_p,

        (ParamFloat<params_id::FW_Y_RMAX>)_param_fw_y_rmax

    )

    ECL_RollController  _roll_ctrl;
    ECL_PitchController _pitch_ctrl;
    ECL_YawController   _yaw_ctrl;
    ECL_WheelController _wheel_ctrl;

    void  parameters_update();
    void  vehicle_manual_poll(const float yaw_body);
    void  vehicle_attitude_setpoint_poll();
    void  vehicle_land_detected_poll();
    float get_airspeed_constrained();
};
