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
#include <rate_control/rate_control.hpp>

// #include <hrtimer.h>
// #include <mathlib/mathlib.h>
// #include <param/param.h>
// #include <perf/perf_counter.h>
#include <slew_rate/SlewRate.hpp>
// #include <matrix/math.hpp>
// #include <px4_platform_common/px4_config.h>

// #include <defines.h>
// #include <module/module_command.hpp>
// #include <module/module_params.hpp>
// #include <px4_platform_common/posix.h>
// #include <px4_platform_common/tasks.h>
// #include <workq/WorkItemScheduled.hpp>
// #include <uORB/uORBPublication.hpp>
// #include <uORB/uORBPublication.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/topics/actuator_controls_status.h>
// #include <uORB/topics/airspeed_validated.h>
// #include <uORB/topics/battery_status.h>
// #include <uORB/topics/control_allocator_status.h>
// #include <uORB/topics/manual_control_setpoint.h>
// #include <uORB/topics/normalized_unsigned_setpoint.h>
// #include <uORB/topics/parameter_update.h>
// #include <uORB/topics/rate_ctrl_status.h>
// #include <uORB/topics/vehicle_angular_velocity.h>
// #include <uORB/topics/vehicle_control_mode.h>
// #include <uORB/topics/vehicle_land_detected.h>
// #include <uORB/topics/vehicle_rates_setpoint.h>
// #include <uORB/topics/vehicle_status.h>
// #include <uORB/topics/vehicle_thrust_setpoint.h>
// #include <uORB/topics/vehicle_torque_setpoint.h>

using matrix::Eulerf;
using matrix::Quatf;

using uORB::SubscriptionData;

using namespace time_literals;

class FixedwingRateControl final : public ModuleCommand<FixedwingRateControl>, public ModuleParams, public WorkItemScheduled {
public:
    FixedwingRateControl(bool vtol = false);
    ~FixedwingRateControl() override;

    /** @see ModuleCommand */
    static FixedwingRateControl *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;

    uORB::SubscriptionCallbackWorkItem _vehicle_angular_velocity_sub{this, ORB_ID(vehicle_angular_velocity)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _battery_status_sub{ORB_ID(battery_status)};
    uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};
    uORB::Subscription _rates_sp_sub{ORB_ID(vehicle_rates_setpoint)};
    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::Subscription _vehicle_land_detected_sub{ORB_ID(vehicle_land_detected)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
    uORB::Subscription _vehicle_rates_sub{ORB_ID(vehicle_angular_velocity)};

    uORB::SubscriptionMultiArray<control_allocator_status_s, 2> _control_allocator_status_subs{ORB_ID::control_allocator_status};

    uORB::SubscriptionData<airspeed_validated_s> _airspeed_validated_sub{ORB_ID(airspeed_validated)};

    uORB::Publication<actuator_controls_status_s>     _actuator_controls_status_pub;
    uORB::Publication<vehicle_rates_setpoint_s>       _rate_sp_pub{ORB_ID(vehicle_rates_setpoint)};
    uORB::PublicationMulti<rate_ctrl_status_s>        _rate_ctrl_status_pub{ORB_ID(rate_ctrl_status)};
    uORB::Publication<vehicle_torque_setpoint_s>      _vehicle_torque_setpoint_pub;
    uORB::Publication<vehicle_thrust_setpoint_s>      _vehicle_thrust_setpoint_pub;
    uORB::Publication<normalized_unsigned_setpoint_s> _flaps_setpoint_pub{ORB_ID(flaps_setpoint)};
    uORB::Publication<normalized_unsigned_setpoint_s> _spoilers_setpoint_pub{ORB_ID(spoilers_setpoint)};

    manual_control_setpoint_s _manual_control_setpoint{};
    vehicle_control_mode_s    _vcontrol_mode{};
    vehicle_thrust_setpoint_s _vehicle_thrust_setpoint{};
    vehicle_torque_setpoint_s _vehicle_torque_setpoint{};
    vehicle_rates_setpoint_s  _rates_sp{};
    vehicle_status_s          _vehicle_status{};

    perf_counter_t _loop_perf;

    hrt_abstime _last_run{0};

    float _airspeed_scaling{1.0f};

    bool _landed{true};

    float _battery_scale{1.0f};

    float _energy_integration_time{0.0f};
    float _control_energy[4]{};
    float _control_prev[3]{};

    bool _in_fw_or_transition_wo_tailsitter_transition{false}; // only run the FW attitude controller in these states

    // enum for bitmask of VT_FW_DIFTHR_EN parameter options
    enum class VTOLFixedWingDifferentialThrustEnabledBit : int32_t {
        YAW_BIT   = (1 << 0),
        ROLL_BIT  = (1 << 1),
        PITCH_BIT = (1 << 2),
    };

    param_t _handle_param_vt_fw_difthr_en{PARAM_INVALID};
    int32_t _param_vt_fw_difthr_en{0};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::FW_ACRO_X_MAX>)_param_fw_acro_x_max,
        (ParamFloat<params_id::FW_ACRO_Y_MAX>)_param_fw_acro_y_max,
        (ParamFloat<params_id::FW_ACRO_Z_MAX>)_param_fw_acro_z_max,

        (ParamFloat<params_id::FW_AIRSPD_MAX>)_param_fw_airspd_max,
        (ParamFloat<params_id::FW_AIRSPD_MIN>)_param_fw_airspd_min,
        (ParamFloat<params_id::FW_AIRSPD_STALL>)_param_fw_airspd_stall,
        (ParamFloat<params_id::FW_AIRSPD_TRIM>)_param_fw_airspd_trim,
        (ParamInt<params_id::FW_ARSP_MODE>)_param_fw_arsp_mode,

        (ParamInt<params_id::FW_ARSP_SCALE_EN>)_param_fw_arsp_scale_en,

        (ParamBool<params_id::FW_BAT_SCALE_EN>)_param_fw_bat_scale_en,

        (ParamFloat<params_id::FW_DTRIM_P_VMAX>)_param_fw_dtrim_p_vmax,
        (ParamFloat<params_id::FW_DTRIM_P_VMIN>)_param_fw_dtrim_p_vmin,
        (ParamFloat<params_id::FW_DTRIM_R_VMAX>)_param_fw_dtrim_r_vmax,
        (ParamFloat<params_id::FW_DTRIM_R_VMIN>)_param_fw_dtrim_r_vmin,
        (ParamFloat<params_id::FW_DTRIM_Y_VMAX>)_param_fw_dtrim_y_vmax,
        (ParamFloat<params_id::FW_DTRIM_Y_VMIN>)_param_fw_dtrim_y_vmin,

        (ParamFloat<params_id::FW_MAN_P_SC>)_param_fw_man_p_sc,
        (ParamFloat<params_id::FW_MAN_R_SC>)_param_fw_man_r_sc,
        (ParamFloat<params_id::FW_MAN_Y_SC>)_param_fw_man_y_sc,

        (ParamFloat<params_id::FW_PR_FF>)_param_fw_pr_ff,
        (ParamFloat<params_id::FW_PR_I>)_param_fw_pr_i,
        (ParamFloat<params_id::FW_PR_IMAX>)_param_fw_pr_imax,
        (ParamFloat<params_id::FW_PR_P>)_param_fw_pr_p,
        (ParamFloat<params_id::FW_PR_D>)_param_fw_pr_d,

        (ParamFloat<params_id::FW_RLL_TO_YAW_FF>)_param_fw_rll_to_yaw_ff,
        (ParamFloat<params_id::FW_RR_FF>)_param_fw_rr_ff,
        (ParamFloat<params_id::FW_RR_I>)_param_fw_rr_i,
        (ParamFloat<params_id::FW_RR_IMAX>)_param_fw_rr_imax,
        (ParamFloat<params_id::FW_RR_P>)_param_fw_rr_p,
        (ParamFloat<params_id::FW_RR_D>)_param_fw_rr_d,

        (ParamFloat<params_id::FW_YR_FF>)_param_fw_yr_ff,
        (ParamFloat<params_id::FW_YR_I>)_param_fw_yr_i,
        (ParamFloat<params_id::FW_YR_IMAX>)_param_fw_yr_imax,
        (ParamFloat<params_id::FW_YR_P>)_param_fw_yr_p,
        (ParamFloat<params_id::FW_YR_D>)_param_fw_yr_d,

        (ParamFloat<params_id::TRIM_PITCH>)_param_trim_pitch,
        (ParamFloat<params_id::TRIM_ROLL>)_param_trim_roll,
        (ParamFloat<params_id::TRIM_YAW>)_param_trim_yaw,

        (ParamInt<params_id::FW_SPOILERS_MAN>)_param_fw_spoilers_man)

    RateControl _rate_control; ///< class for rate control calculations

    void updateActuatorControlsStatus(float dt);

    /**
     * Update our local parameter cache.
     */
    int parameters_update();

    void vehicle_manual_poll();
    void vehicle_land_detected_poll();

    float get_airspeed_and_update_scaling();
};
