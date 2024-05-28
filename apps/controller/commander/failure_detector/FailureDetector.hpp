
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
 * @file FailureDetector.hpp
 * Base class for failure detection logic based on vehicle states
 * for failsafe triggering.
 *
 * @author Mathieu Bresciani 	<brescianimathieu@gmail.com>
 *
 */

#pragma once

#include <hysteresis/hysteresis.h>
#include <mathlib/mathlib.h>
#include <mathlib/math/filter/AlphaFilter.hpp>
#include <matrix/matrix/math.hpp>
#include <module/module_params.hpp>

// subscriptions
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/actuator_motors.h>
#include <uORB/topics/sensor_selection.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_imu_status.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/esc_status.h>
#include <uORB/topics/pwm_input.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

union failure_detector_status_u {
    struct {
        uint16_t roll            : 1;
        uint16_t pitch           : 1;
        uint16_t alt             : 1;
        uint16_t ext             : 1;
        uint16_t arm_escs        : 1;
        uint16_t battery         : 1;
        uint16_t imbalanced_prop : 1;
        uint16_t motor           : 1;
    } flags;

    uint16_t value{0};
};

using uORB::SubscriptionData;

class FailureInjector {
public:
    void update();

    void manipulateEscStatus(esc_status_s &status);

private:
    uORB::Subscription                       _vehicle_command_sub{ORB_ID(vehicle_command)};
    uORB::Publication<vehicle_command_ack_s> _command_ack_pub{ORB_ID(vehicle_command_ack)};

    uint32_t _esc_blocked{};
    uint32_t _esc_wrong{};
};

class FailureDetector : public ModuleParams {
public:
    FailureDetector(ModuleParams *parent);
    ~FailureDetector() = default;

    bool update(const vehicle_status_s &vehicle_status, const vehicle_control_mode_s &vehicle_control_mode);

    const failure_detector_status_u &getStatus() const {
        return _status;
    }

    const decltype(failure_detector_status_u::flags) &getStatusFlags() const {
        return _status.flags;
    }

    float getImbalancedPropMetric() const {
        return _imbalanced_prop_lpf.getState();
    }

    uint16_t getMotorFailures() const {
        return _motor_failure_esc_timed_out_mask | _motor_failure_esc_under_current_mask;
    }

private:
    void updateAttitudeStatus(const vehicle_status_s &vehicle_status);
    void updateExternalAtsStatus();
    void updateEscsStatus(const vehicle_status_s &vehicle_status, const esc_status_s &esc_status);
    void updateMotorStatus(const vehicle_status_s &vehicle_status, const esc_status_s &esc_status);
    void updateImbalancedPropStatus();

    failure_detector_status_u _status{};

    Hysteresis _roll_failure_hysteresis{false};
    Hysteresis _pitch_failure_hysteresis{false};
    Hysteresis _ext_ats_failure_hysteresis{false};
    Hysteresis _esc_failure_hysteresis{false};

    static constexpr float _imbalanced_prop_lpf_time_constant{5.f};
    AlphaFilter<float>     _imbalanced_prop_lpf{};
    uint32_t               _selected_accel_device_id{0};
    hrt_abstime            _imu_status_timestamp_prev{0};

    // Motor failure check
    uint8_t     _motor_failure_esc_valid_current_mask{}; // ESC 1-8, true if ESC telemetry was valid at some point
    uint8_t     _motor_failure_esc_timed_out_mask{};     // ESC telemetry no longer available -> failure
    uint8_t     _motor_failure_esc_under_current_mask{}; // ESC drawing too little current -> failure
    bool        _motor_failure_escs_have_current{false}; // true if some ESC had non-zero current (some don't support it)
    hrt_abstime _motor_failure_undercurrent_start_time[actuator_motors_s::NUM_CONTROLS]{};

    uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _esc_status_sub{ORB_ID(esc_status)}; // TODO: multi-instance
    uORB::Subscription _pwm_input_sub{ORB_ID(pwm_input)};
    uORB::Subscription _sensor_selection_sub{ORB_ID(sensor_selection)};
    uORB::Subscription _vehicle_imu_status_sub{ORB_ID(vehicle_imu_status)};
    uORB::Subscription _actuator_motors_sub{ORB_ID(actuator_motors)};

    FailureInjector _failure_injector;

    DEFINE_PARAMETERS(
        (ParamInt<params_id::FD_FAIL_P>)_param_fd_fail_p,
        (ParamInt<params_id::FD_FAIL_R>)_param_fd_fail_r,
        (ParamFloat<params_id::FD_FAIL_R_TTRI>)_param_fd_fail_r_ttri,
        (ParamFloat<params_id::FD_FAIL_P_TTRI>)_param_fd_fail_p_ttri,
        (ParamBool<params_id::FD_EXT_ATS_EN>)_param_fd_ext_ats_en,
        (ParamInt<params_id::FD_EXT_ATS_TRIG>)_param_fd_ext_ats_trig,
        (ParamInt<params_id::FD_ESCS_EN>)_param_escs_en,
        (ParamInt<params_id::FD_IMB_PROP_THR>)_param_fd_imb_prop_thr,

        // Actuator failure
        (ParamBool<params_id::FD_ACT_EN>)_param_fd_actuator_en,
        (ParamFloat<params_id::FD_ACT_MOT_THR>)_param_fd_motor_throttle_thres,
        (ParamFloat<params_id::FD_ACT_MOT_C2T>)_param_fd_motor_current2throttle_thres,
        (ParamInt<params_id::FD_ACT_MOT_TOUT>)_param_fd_motor_time_thres)
};
