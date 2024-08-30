/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "EscBattery.hpp"

#include <math.h>

using namespace time_literals;

EscBattery::EscBattery() :
    ModuleParams(nullptr),
    WorkItem(MODULE_NAME, nextpilot::wq_configurations::lp_default),
    _battery(1, this, ESC_BATTERY_INTERVAL_US, battery_status_s::BATTERY_SOURCE_ESCS) {
}

bool EscBattery::init() {
    if (!_esc_status_sub.registerCallback()) {
        LOG_E("callback registration failed");
        return false;
    }

    _esc_status_sub.set_interval_us(ESC_BATTERY_INTERVAL_US);

    return true;
}

void EscBattery::parameters_updated() {
    ModuleParams::updateParams();
}

void EscBattery::Run() {
    if (should_exit()) {
        _esc_status_sub.unregisterCallback();
        exit_and_cleanup();
        return;
    }

    if (_parameter_update_sub.updated()) {
        // Clear update
        parameter_update_s param_update;
        _parameter_update_sub.copy(&param_update);

        parameters_updated();
    }

    esc_status_s esc_status;

    if (_esc_status_sub.copy(&esc_status)) {
        if (esc_status.esc_count == 0 || esc_status.esc_count > esc_status_s::CONNECTED_ESC_MAX) {
            return;
        }

        const uint8_t online_esc_count  = math::countSetBits(esc_status.esc_online_flags);
        float         average_voltage_v = 0.0f;
        float         total_current_a   = 0.0f;

        for (unsigned i = 0; i < esc_status.esc_count; ++i) {
            if ((1 << i) & esc_status.esc_online_flags) {
                average_voltage_v += esc_status.esc[i].esc_voltage;
                total_current_a   += esc_status.esc[i].esc_current;
            }
        }

        average_voltage_v /= online_esc_count;

        _battery.setConnected(true);
        _battery.updateVoltage(average_voltage_v);
        _battery.updateCurrent(total_current_a);
        _battery.updateAndPublishBatteryStatus(esc_status.timestamp);
    }
}

int EscBattery::task_spawn(int argc, char *argv[]) {
    EscBattery *instance = new EscBattery();

    if (instance) {
        _object.store(instance);
        _task_id = task_id_is_work_queue;

        if (instance->init()) {
            return RT_EOK;
        }

    } else {
        LOG_E("alloc failed");
    }

    delete instance;
    _object.store(nullptr);
    _task_id = -1;

    return RT_ERROR;
}

int EscBattery::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int EscBattery::print_usage(const char *reason) {
    if (reason) {
        LOG_W("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
This implements using information from the ESC status and publish it as battery status.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("esc_battery", "system");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

extern "C" __EXPORT int esc_battery_main(int argc, char *argv[]) {
    return EscBattery::main(argc, argv);
}
