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

#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.h>
#include <workq/ScheduledWorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/SubscriptionInterval.hpp>
#include <uORB/SubscriptionCallback.hpp>
#include <uORB/topics/esc_status.h>
#include <uORB/topics/parameter_update.h>
#include <battery/battery.h>

using namespace time_literals;

class EscBattery : public ModuleBase<EscBattery>, public ModuleParams, public px4::WorkItem {
public:
    EscBattery();
    ~EscBattery() = default;

    /** @see ModuleBase */
    static int task_spawn(int argc, char *argv[]);

    /** @see ModuleBase */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleBase */
    static int print_usage(const char *reason = nullptr);

    bool init();

private:
    void Run() override;

    void parameters_updated();

    uORB::SubscriptionInterval         _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::SubscriptionCallbackWorkItem _esc_status_sub{this, ORB_ID(esc_status)};

    static constexpr uint32_t ESC_BATTERY_INTERVAL_US = 20_ms; // assume higher frequency esc feedback than 50Hz
    Battery                   _battery;
};
