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

#include <battery/battery.h>
#include <parameters/param.h>

class AnalogBattery : public Battery {
public:
    AnalogBattery(int index, ModuleParams *parent, const int sample_interval_us, const uint8_t source,
                  const uint8_t priority);

    /**
     * Update current battery status message.
     *
     * @param voltage_raw Battery voltage read from ADC, volts
     * @param current_raw Voltage of current sense resistor, volts
     * @param timestamp Time at which the ADC was read (use hrt_absolute_time())
     * @param source The source as defined by param BAT%d_SOURCE
     * @param priority: The brick number -1. The term priority refers to the Vn connection on the LTC4417
     */
    void updateBatteryStatusADC(hrt_abstime timestamp, float voltage_raw, float current_raw);

    /**
     * Whether the ADC channel for the voltage of this battery is valid.
     * Corresponds to BOARD_BRICK_VALID_LIST
     */
    bool is_valid();

    /**
     * Which ADC channel is used for voltage reading of this battery
     */
    int get_voltage_channel();

    /**
     * Which ADC channel is used for current reading of this battery
     */
    int get_current_channel();

protected:
    struct {
        param_t v_offs_cur;
        param_t v_div;
        param_t a_per_v;
        param_t v_channel;
        param_t i_channel;
    } _analog_param_handles;

    struct {
        float   v_offs_cur;
        float   v_div;
        float   a_per_v;
        int32_t v_channel;
        int32_t i_channel;
    } _analog_params;

    virtual void updateParams() override;
};
