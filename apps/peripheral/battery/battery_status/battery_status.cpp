/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <rtdevice.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <getopt/getopt.h>
#include <ulog/log.h>
#include <mathlib/mathlib.h>
#include <hrtimer.h>
#include <param/param.h>
#include <perf/perf_counter.h>
#include <battery/battery.h>
#include <conversion/rotation.h>
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/parameter_update.h>
#include <workq/WorkItemScheduled.hpp>


#include "analog_battery.h"

using namespace time_literals;
using namespace nextpilot::workq;

/**
 * The channel definitions (e.g., ADC_BATTERY_VOLTAGE_CHANNEL, ADC_BATTERY_CURRENT_CHANNEL, and ADC_AIRSPEED_VOLTAGE_CHANNEL) are defined in board_config.h
 */


#ifndef BOARD_NUMBER_BRICKS
#   error "battery_status module requires power bricks"
#endif

#if BOARD_NUMBER_BRICKS == 0
#   error "battery_status module requires power bricks"
#endif

class BatteryStatus : public ModuleCommand<BatteryStatus>, public ModuleParams, public WorkItemScheduled {
public:
    BatteryStatus();
    ~BatteryStatus() override;

    /** @see ModuleCommand */
    // static int task_spawn(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

    int print_status() override;

private:
    void Run() override;

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s}; /**< notification of parameter updates */
    // uORB::SubscriptionCallbackWorkItem _adc_report_sub{this, ORB_ID(adc_report)};

    static constexpr uint32_t SAMPLE_FREQUENCY_HZ = 100;
    static constexpr uint32_t SAMPLE_INTERVAL_US  = 1_s / SAMPLE_FREQUENCY_HZ;

    rt_adc_device_t _device{nullptr};

    AnalogBattery _battery1;

#if BOARD_NUMBER_BRICKS > 1
    AnalogBattery _battery2;
#endif

    AnalogBattery *_analogBatteries[BOARD_NUMBER_BRICKS]{
        &_battery1,
#if BOARD_NUMBER_BRICKS > 1
        &_battery2,
#endif
    }; // End _analogBatteries

    perf_counter_t _loop_perf; /**< loop performance counter */

    /**
     * Check for changes in parameters.
     */
    void parameter_update_poll(bool forced = false);

    /**
     * Poll the ADC and update readings to suit.
     *
     * @param raw			Combined sensor data structure into which
     *				data should be returned.
     */
    void adc_poll();
};

BatteryStatus::BatteryStatus() :
    ModuleParams(nullptr),
    WorkItemScheduled(MODULE_NAME, wq_configurations::hp_default),
    _battery1(1, this, SAMPLE_INTERVAL_US, battery_status_s::BATTERY_SOURCE_POWER_MODULE, 0),
#if BOARD_NUMBER_BRICKS > 1
    _battery2(2, this, SAMPLE_INTERVAL_US, battery_status_s::BATTERY_SOURCE_POWER_MODULE, 1),
#endif
    _loop_perf(perf_alloc(PC_ELAPSED, MODULE_NAME)) {
    updateParams();
}

BatteryStatus::~BatteryStatus() {
    ScheduleClear();
}

void BatteryStatus::parameter_update_poll(bool forced) {
    // check for parameter updates
    if (_parameter_update_sub.updated() || forced) {
        // clear update
        parameter_update_s pupdate;
        _parameter_update_sub.copy(&pupdate);

        // update parameters from storage
        updateParams();
    }
}

void BatteryStatus::adc_poll() {
    /* For legacy support we publish the battery_status for the Battery that is
     * associated with the Brick that is the selected source for VDD_5V_IN
     * Selection is done in HW ala a LTC4417 or similar, or maybe hard coded
     * Like in the FMUv4
     */

    /* Per Brick readings with default unread channels at 0 */
    float bat_current_adc_readings[BOARD_NUMBER_BRICKS]{};
    float bat_voltage_adc_readings[BOARD_NUMBER_BRICKS]{};
    bool  has_bat_voltage_adc_channel[BOARD_NUMBER_BRICKS]{};

// 获取ADC分辨率
#if defined(SOC_SERIES_STM32H7)
#   define RESOLUTION (3.3f / (1 << 16))
#elif defined(SOC_SERIES_STM32F7)
#   define RESOLUTION (3.3f / (1 << 12))
#else
#   error please set adc resolution
#endif

    // 读取ADC采集结果
    bat_voltage_adc_readings[0]    = rt_adc_read(_device, ADC_BATTERY1_VOLTAGE_CHANNEL) * RESOLUTION;
    bat_current_adc_readings[0]    = rt_adc_read(_device, ADC_BATTERY1_CURRENT_CHANNEL) * RESOLUTION;
    has_bat_voltage_adc_channel[0] = true;

#if BOARD_NUMBER_BRICKS > 1
    bat_voltage_adc_readings[1]    = rt_adc_read(_device, ADC_BATTERY2_VOLTAGE_CHANNEL) * RESOLUTION;
    bat_current_adc_readings[1]    = rt_adc_read(_device, ADC_BATTERY2_CURRENT_CHANNEL) * RESOLUTION;
    has_bat_voltage_adc_channel[1] = true;
#endif // BOARD_NUMBER_BRICKS

    for (int b = 0; b < BOARD_NUMBER_BRICKS; b++) {
        if (has_bat_voltage_adc_channel[b]) { // Do not publish if no voltage channel configured
            _analogBatteries[b]->updateBatteryStatusADC(
                hrt_absolute_time(),
                bat_voltage_adc_readings[b],
                bat_current_adc_readings[b]);
        }
    }
}

void BatteryStatus::Run() {
    if (should_exit()) {
        exit_and_cleanup();
        return;
    }

    perf_begin(_loop_perf);

    /* check parameters for updates */
    parameter_update_poll();

    /* check battery voltage */
    adc_poll();

    perf_end(_loop_perf);
}

// int BatteryStatus::task_spawn(int argc, char *argv[]) {
//     BatteryStatus *instance = new BatteryStatus();

//     if (instance) {
//         _object.store(instance);
//         _task_id = task_id_is_work_queue;

//         if (instance->init()) {
//             return RT_EOK;
//         }

//     } else {
//         LOG_E("alloc failed");
//     }

//     delete instance;
//     _object.store(nullptr);
//     _task_id = -1;

//     return RT_ERROR;
// }

int BatteryStatus::init() {
    _device = (rt_adc_device_t)rt_device_find(ADC_BATTERY_DEVICE_NAME);
    if (!_device) {
        LOG_E("can't find: %s", ADC_BATTERY_DEVICE_NAME);
        return -1;
    }

    rt_adc_enable(_device, ADC_BATTERY1_VOLTAGE_CHANNEL);
    rt_adc_enable(_device, ADC_BATTERY1_CURRENT_CHANNEL);

#if BOARD_NUMBER_BRICKS > 1
    rt_adc_enable(_device, ADC_BATTERY2_VOLTAGE_CHANNEL);
    rt_adc_enable(_device, ADC_BATTERY2_CURRENT_CHANNEL);
#endif // BOARD_NUMBER_BRICKS

    // rt_adc_enable(_device,ADC_SPARE1_CHANNEL);
    // rt_adc_enable(_device,ADC_BATTERY1_CURRENT_CHANNEL);

    // 100hz速度读取adc
    ScheduleOnInterval(SAMPLE_INTERVAL_US, 0);

    return RT_EOK;
}

int BatteryStatus::print_status() {
    for (int b = 0; b < BOARD_NUMBER_BRICKS; b++) {
        // _analogBatteries[b]->print_status();
    }
    return 0;
}

int BatteryStatus::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int BatteryStatus::print_usage(const char *reason) {
    if (reason) {
        LOG_W("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description

The provided functionality includes:
- Read the output from the ADC driver (via ioctl interface) and publish `battery_status`.


### Implementation
It runs in its own thread and polls on the currently selected gyro topic.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("battery_status", "system");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

static int battery_status_main(int argc, char *argv[]) {
    return BatteryStatus::main(argc, argv);
}

MSH_CMD_EXPORT_ALIAS(battery_status_main, battery_low_remaining_time, battery status);

static int battery_status_start() {
    int32_t sys_hitl = param_get_int32((param_t)nextpilot::param::params_id::SYS_HITL);
    if (sys_hitl > 0) {
        return 0;
    }

    const char *argv[] = {"battery_status", "start"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);
    return BatteryStatus::main(argc, (char **)argv);
}

INIT_APP_EXPORT(battery_status_start);
