/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PWMOUT_H__
#define __PWMOUT_H__


#pragma once

#include <float.h>
#include <math.h>

// #include <board_config.h>

#include <hrtimer.h>
// #include <drivers/drv_pwm_output.h>
#include <mathlib/mathlib.h>
#include <mixer_module/mixer_module.hpp>
#include <perf/perf_counter.h>
// #include <px4_arch/io_timer.h>
// #include <px4_platform_common/px4_config.h>
#include <getopt/getopt.h>
// #include <px4_platform_common/log.h>
// #include <px4_platform_common/module.h>
#include <module/module_params.h>
#include <module/module_command.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/parameter_update.h>

using namespace time_literals;

class PWMOut final : public ModuleCommand<PWMOut>, public OutputModuleInterface {
public:
    PWMOut();
    ~PWMOut() override;

    /** @see ModuleBase */
    // static int task_spawn(int argc, char *argv[]);

    /** @see ModuleBase */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleBase */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleBase::print_status() */
    int print_status() override;

    bool updateOutputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS],
                       unsigned num_outputs, unsigned num_control_groups_updated) override;

private:
    void Run() override;

    void update_params();
    bool update_pwm_out_state(bool on);

    MixingOutput _mixing_output{PARAM_PREFIX, DIRECT_PWM_OUTPUT_CHANNELS, *this, MixingOutput::SchedulingPolicy::Auto, true};

    int _timer_rates[MAX_IO_TIMERS]{};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    unsigned _num_outputs{DIRECT_PWM_OUTPUT_CHANNELS};

    bool     _pwm_on{false};
    uint32_t _pwm_mask{0};
    bool     _pwm_initialized{false};
    bool     _first_update_cycle{true};

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};
    perf_counter_t _interval_perf{perf_alloc(PC_INTERVAL, MODULE_NAME ": interval")};
};

#endif // __PWMOUT_H__
