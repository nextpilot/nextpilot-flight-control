/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <stdio.h>
#include <battery/battery.h>
#include "analog_battery.h"

// Defaults to use if the parameters are not set
#if BOARD_NUMBER_BRICKS > 0
#   if defined(BOARD_BATT_V_LIST) && defined(BOARD_BATT_I_LIST)
static constexpr int DEFAULT_V_CHANNEL[BOARD_NUMBER_BRICKS] = BOARD_BATT_V_LIST;
static constexpr int DEFAULT_I_CHANNEL[BOARD_NUMBER_BRICKS] = BOARD_BATT_I_LIST;
#   else
#      error "BOARD_BATT_V_LIST and BOARD_BATT_I_LIST need to be defined"
#   endif
#else
static constexpr int DEFAULT_V_CHANNEL[1] = {-1};
static constexpr int DEFAULT_I_CHANNEL[1] = {-1};
#endif

/* Define the default Under voltage Window on the LTC4417 as set by resistors on the
 * board. Default is that of the FMUv2 at 3.7V
 */
#ifndef BOARD_VALID_UV
#   define BOARD_VALID_UV (3.7f)
#endif // BOARD_VALID_UV
/* HW has to large of R termination on ADC todo:change when HW value is chosen */
#ifndef BOARD_ADC_OPEN_CIRCUIT_V
#   define BOARD_ADC_OPEN_CIRCUIT_V (5.6f)
#endif // BOARD_ADC_OPEN_CIRCUIT_V

AnalogBattery::AnalogBattery(int index, ModuleParams *parent, const int sample_interval_us, const uint8_t source,
                             const uint8_t priority) :
    Battery(index, parent, sample_interval_us, source) {
    Battery::setPriority(priority);
    char param_name[17];

    _analog_param_handles.v_offs_cur = param_find("BAT_V_OFFS_CURR");

    snprintf(param_name, sizeof(param_name), "BAT%d_V_DIV", index);
    _analog_param_handles.v_div = param_find(param_name);

    snprintf(param_name, sizeof(param_name), "BAT%d_A_PER_V", index);
    _analog_param_handles.a_per_v = param_find(param_name);

    snprintf(param_name, sizeof(param_name), "BAT%d_V_CHANNEL", index);
    _analog_param_handles.v_channel = param_find(param_name);

    snprintf(param_name, sizeof(param_name), "BAT%d_I_CHANNEL", index);
    _analog_param_handles.i_channel = param_find(param_name);
}

void AnalogBattery::updateBatteryStatusADC(hrt_abstime timestamp, float voltage_raw, float current_raw) {
    const float voltage_v = voltage_raw * _analog_params.v_div;
    const float current_a = (current_raw - _analog_params.v_offs_cur) * _analog_params.a_per_v;

    const bool connected = voltage_v > BOARD_ADC_OPEN_CIRCUIT_V && (BOARD_ADC_OPEN_CIRCUIT_V <= BOARD_VALID_UV || is_valid());

    Battery::setConnected(connected);
    Battery::updateVoltage(voltage_v);
    Battery::updateCurrent(current_a);
    Battery::updateAndPublishBatteryStatus(timestamp);
}

bool AnalogBattery::is_valid() {
#ifdef BOARD_BRICK_VALID_LIST
    bool valid[BOARD_NUMBER_BRICKS] = BOARD_BRICK_VALID_LIST;
    return valid[_index - 1];
#else
    // TODO: Maybe return false instead?
    return true;
#endif
}

int AnalogBattery::get_voltage_channel() {
    if (_analog_params.v_channel >= 0) {
        return _analog_params.v_channel;

    } else {
        return DEFAULT_V_CHANNEL[_index - 1];
    }
}

int AnalogBattery::get_current_channel() {
    if (_analog_params.i_channel >= 0) {
        return _analog_params.i_channel;

    } else {
        return DEFAULT_I_CHANNEL[_index - 1];
    }
}

void AnalogBattery::updateParams() {
    param_get(_analog_param_handles.v_div, &_analog_params.v_div);
    param_get(_analog_param_handles.a_per_v, &_analog_params.a_per_v);
    param_get(_analog_param_handles.v_channel, &_analog_params.v_channel);
    param_get(_analog_param_handles.i_channel, &_analog_params.i_channel);
    param_get(_analog_param_handles.v_offs_cur, &_analog_params.v_offs_cur);

    Battery::updateParams();
}
