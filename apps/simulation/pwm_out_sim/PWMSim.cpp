/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "sim_pwm_out"

#include "PWMSim.hpp"
#include <mathlib/mathlib.h>
#include <getopt/getopt.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/parameter_update.h>

PWMSim::PWMSim(bool hil_mode_enabled) :
    OutputModuleInterface(MODULE_NAME, wq_configurations::hp_default) {
    _mixing_output.setAllDisarmedValues(PWM_SIM_DISARMED_MAGIC);
    _mixing_output.setAllFailsafeValues(PWM_SIM_FAILSAFE_MAGIC);
    _mixing_output.setAllMinValues(PWM_SIM_PWM_MIN_MAGIC);
    _mixing_output.setAllMaxValues(PWM_SIM_PWM_MAX_MAGIC);

    _mixing_output.setIgnoreLockdown(hil_mode_enabled);
}

PWMSim::~PWMSim() {
    perf_free(_cycle_perf);
    perf_free(_interval_perf);
}

bool PWMSim::updateOutputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs,
                           unsigned num_control_groups_updated) {
    // Only publish once we receive actuator_controls (important for lock-step to work correctly)
    if (num_control_groups_updated > 0) {
        actuator_outputs_s actuator_outputs{};
        actuator_outputs.noutputs = num_outputs;

        const uint32_t reversible_outputs = _mixing_output.reversibleOutputs();

        for (int i = 0; i < (int)num_outputs; i++) {
            if (outputs[i] != PWM_SIM_DISARMED_MAGIC) {
                OutputFunction function      = _mixing_output.outputFunction(i);
                bool           is_reversible = reversible_outputs & (1u << i);
                float          output        = outputs[i];

                if (((int)function >= (int)OutputFunction::Motor1 && (int)function <= (int)OutputFunction::MotorMax) && !is_reversible) {
                    // Scale non-reversible motors to [0, 1]
                    actuator_outputs.output[i] = (output - PWM_SIM_PWM_MIN_MAGIC) / (PWM_SIM_PWM_MAX_MAGIC - PWM_SIM_PWM_MIN_MAGIC);

                } else {
                    // Scale everything else to [-1, 1]
                    const float pwm_center     = (PWM_SIM_PWM_MAX_MAGIC + PWM_SIM_PWM_MIN_MAGIC) / 2.f;
                    const float pwm_delta      = (PWM_SIM_PWM_MAX_MAGIC - PWM_SIM_PWM_MIN_MAGIC) / 2.f;
                    actuator_outputs.output[i] = (output - pwm_center) / pwm_delta;
                }
            }
        }

        actuator_outputs.timestamp = hrt_absolute_time();
        _actuator_outputs_sim_pub.publish(actuator_outputs);
        return true;
    }

    return false;
}

void PWMSim::Run() {
    if (should_exit()) {
        ScheduleClear();
        _mixing_output.unregister();

        exit_and_cleanup();
        return;
    }

    _mixing_output.update();

    // check for parameter updates
    if (_parameter_update_sub.updated()) {
        parameter_update_s pupdate;
        _parameter_update_sub.copy(&pupdate);
        updateParams();
    }

    // check at end of cycle (updateSubscriptions() can potentially change to a different WorkQueue thread)
    _mixing_output.updateSubscriptions(true);
}

PWMSim *PWMSim::instantiate(int argc, char *argv[]) {
    bool hil_mode = false;

    int         myoptind = 1;
    int         ch;
    const char *myoptarg = nullptr;

    while ((ch = getopt(argc, argv, "m:", &myoptind, &myoptarg)) != EOF) {
        switch (ch) {
        case 'm':
            hil_mode = rt_strcmp(myoptarg, "hil") == 0;
            break;

        default:
            print_usage("unrecognized flag");
            return nullptr;
        }
    }

    PWMSim *instance = new PWMSim(hil_mode);

    if (!instance) {
        PX4_ERR("alloc failed");
        return nullptr;
    }

    // _object.store(instance);
    // _task_id = task_id_is_work_queue;
    // instance->ScheduleNow();
    return instance;
}

int PWMSim::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int PWMSim::print_status() {
    perf_print_counter(_cycle_perf);
    perf_print_counter(_interval_perf);
    _mixing_output.printStatus();

    return 0;
}

int PWMSim::print_usage(const char *reason) {
    if (reason) {
        PX4_WARN("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
Driver for simulated PWM outputs.

Its only function is to take `actuator_control` uORB messages,
mix them with any loaded mixer and output the result to the
`actuator_output` uORB topic.

It is used in SITL and HITL.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("pwm_out_sim", "driver");
    PRINT_MODULE_USAGE_COMMAND_DESCR("start", "Start the module");
    PRINT_MODULE_USAGE_PARAM_STRING('m', "sim", "hil|sim", "Mode", true);
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

extern "C" __EXPORT int pwm_out_sim_main(int argc, char *argv[]) {
    return PWMSim::main(argc, argv);
}

MSH_CMD_EXPORT_ALIAS(pwm_out_sim_main, sim_pwm_out, pwm out simulator);

int pwm_out_sim_start() {
    int32_t hitl = param_get_int32((param_t)params_id::SYS_HITL);

    if (hitl == 2) {
        const char *argv[] = {"sim_pwm_out", "start"};
        int         argc   = sizeof(argv) / sizeof(argv[0]);
        return PWMSim::main(argc, (char **)argv);
    }

    return 0;
}

INIT_APP_EXPORT(pwm_out_sim_start);
