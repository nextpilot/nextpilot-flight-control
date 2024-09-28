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
 * @file hover_thrust_estimator.hpp
 * @brief Interface class for a hover thrust estimator
 * Convention is positive thrust, hover thrust and acceleration UP
 *
 * @author Mathieu Bresciani 	<brescianimathieu@gmail.com>
 */

#pragma once

#include <hrtimer.h>
#include <hysteresis/hysteresis.h>
#include <perf/perf_counter.h>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>

#include <uORB/topics/hover_thrust_estimate.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_land_detected.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_local_position_setpoint.h>
#include <uORB/topics/vehicle_status.h>

#include "zero_order_hover_thrust_ekf.hpp"

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::workq;
using namespace nextpilot::param;

class MulticopterHoverThrustEstimator : public ModuleCommand<MulticopterHoverThrustEstimator>, public ModuleParams, public WorkItem {
public:
    MulticopterHoverThrustEstimator();
    ~MulticopterHoverThrustEstimator() override;

    /** @see ModuleCommand */
    static MulticopterHoverThrustEstimator *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

    /** @see ModuleCommand::print_status() */
    int print_status() override;

private:
    void Run() override;
    void updateParams() override;

    void reset();

    void publishStatus(const hrt_abstime &timestamp_sample);
    void publishInvalidStatus();

    ZeroOrderHoverThrustEkf _hover_thrust_ekf{};

    uORB::Publication<hover_thrust_estimate_s> _hover_thrust_ekf_pub{ORB_ID(hover_thrust_estimate)};

    uORB::SubscriptionCallbackWorkItem _vehicle_local_position_sub{this, ORB_ID(vehicle_local_position)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _vehicle_land_detected_sub{ORB_ID(vehicle_land_detected)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
    uORB::Subscription _vehicle_local_position_setpoint_sub{ORB_ID(vehicle_local_position_setpoint)};

    hrt_abstime _timestamp_last{0};

    bool _armed{false};
    bool _landed{false};
    bool _in_air{false};

    bool _valid{false};

    Hysteresis _valid_hysteresis{false};

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle time")};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::HTE_HT_NOISE>)_param_hte_ht_noise,
        (ParamFloat<params_id::HTE_ACC_GATE>)_param_hte_acc_gate,
        (ParamFloat<params_id::HTE_HT_ERR_INIT>)_param_hte_ht_err_init,
        (ParamFloat<params_id::HTE_THR_RANGE>)_param_hte_thr_range,
        (ParamFloat<params_id::HTE_VXY_THR>)_param_hte_vxy_thr,
        (ParamFloat<params_id::HTE_VZ_THR>)_param_hte_vz_thr,
        (ParamFloat<params_id::MPC_THR_HOVER>)_param_mpc_thr_hover)
};
