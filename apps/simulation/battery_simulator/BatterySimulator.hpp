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

#include <lib/battery/battery.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <module_params.hpp>
#include <px4_platform_common/px4_work_queue/WorkItemScheduled.hpp>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/SubscriptionInterval.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>

using namespace time_literals;

class BatterySimulator : public ModuleCommand<BatterySimulator>, public ModuleParams, public WorkItemScheduled {
public:
    BatterySimulator();
    ~BatterySimulator() override;

    /** @see ModuleCommand */
    static int *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;
    void updateCommands();

    static constexpr uint32_t BATTERY_SIMLATOR_SAMPLE_FREQUENCY_HZ = 100; // Hz
    static constexpr uint32_t BATTERY_SIMLATOR_SAMPLE_INTERVAL_US  = 1_s / BATTERY_SIMLATOR_SAMPLE_FREQUENCY_HZ;

    uORB::Publication<battery_status_s> _battery_pub{ORB_ID(battery_status)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::Subscription         _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::Subscription                       _vehicle_command_sub{ORB_ID(vehicle_command)};
    uORB::Publication<vehicle_command_ack_s> _command_ack_pub{ORB_ID(vehicle_command_ack)};

    Battery _battery;

    uint64_t _last_integration_us{0};
    float    _battery_percentage{1.f};
    bool     _armed{false};

    bool _force_empty_battery{false};

    perf_counter_t _loop_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::SIM_BAT_DRAIN>)_param_sim_bat_drain, ///< battery drain interval
        (ParamFloat<params_id::SIM_BAT_MIN_PCT>)_param_bat_min_pct  //< minimum battery percentage
    )
};
