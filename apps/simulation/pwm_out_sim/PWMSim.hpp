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

#include <drivers/device/device.h>
#include <hrtimer.h>
#include <drivers/drv_pwm_output.h>
#include <mixer_module/mixer_module.hpp>
// #include <px4_platform_common/px4_config.h>
#include <module/module_command.hpp>
#include <px4_platform_common/tasks.h>
#include <px4_platform_common/time.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/uORBSubscription.hpp>

#if defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#else
#define PARAM_PREFIX "HIL_ACT"
#endif

using namespace time_literals;

class PWMSim : public ModuleCommand<PWMSim>, public OutputModuleInterface {
public:
    PWMSim(bool hil_mode_enabled);
    ~PWMSim() override;

    /** @see ModuleCommand */
    static int *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    bool updateOutputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS],
                       unsigned num_outputs, unsigned num_control_groups_updated) override;

private:
    void Run() override;

    static constexpr uint16_t PWM_SIM_DISARMED_MAGIC = 900;
    static constexpr uint16_t PWM_SIM_FAILSAFE_MAGIC = 600;
    static constexpr uint16_t PWM_SIM_PWM_MIN_MAGIC  = 1000;
    static constexpr uint16_t PWM_SIM_PWM_MAX_MAGIC  = 2000;

    MixingOutput               _mixing_output{PARAM_PREFIX, MAX_ACTUATORS, *this, MixingOutput::SchedulingPolicy::Auto, false, false};
    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Publication<actuator_outputs_s> _actuator_outputs_sim_pub{ORB_ID(actuator_outputs_sim)};

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};
    perf_counter_t _interval_perf{perf_alloc(PC_INTERVAL, MODULE_NAME ": interval")};
};
