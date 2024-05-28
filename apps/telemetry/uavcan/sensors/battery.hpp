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
 * @author Jacob Dahl <dahl.jakejacob@gmail.com>
 * @author Alex Klimaj <alex@arkelectron.com>
 */

#pragma once

#include "sensor_bridge.hpp"
#include <uORB/topics/battery_status.h>
#include <uavcan/equipment/power/BatteryInfo.hpp>
#include <ardupilot/equipment/power/BatteryInfoAux.hpp>
#include <battery/battery.h>
#include <hrtimer.h>
#include <module/module_params.hpp>

using namespace time_literals;

class UavcanBatteryBridge : public UavcanSensorBridgeBase, public ModuleParams {
public:
    static const char *const NAME;

    UavcanBatteryBridge(uavcan::INode &node);

    const char *get_name() const override {
        return NAME;
    }

    int init() override;

private:
    /* Different options to update the battery status */
    enum class BatteryDataType {
        Raw,    // data from BatteryInfo message only
        RawAux, // data combination from BatteryInfo and BatteryInfoAux messages
        Filter, // filter data from BatteryInfo message with Battery library
    };

    void battery_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::power::BatteryInfo> &msg);
    void battery_aux_sub_cb(const uavcan::ReceivedDataStructure<ardupilot::equipment::power::BatteryInfoAux> &msg);
    void sumDischarged(hrt_abstime timestamp, float current_a);
    void determineWarning(float remaining);
    void filterData(const uavcan::ReceivedDataStructure<uavcan::equipment::power::BatteryInfo> &msg, uint8_t instance);

    typedef uavcan::MethodBinder<UavcanBatteryBridge *,
                                 void (UavcanBatteryBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::power::BatteryInfo> &)>
        BatteryInfoCbBinder;
    typedef uavcan::MethodBinder<UavcanBatteryBridge *,
                                 void (UavcanBatteryBridge::*)(const uavcan::ReceivedDataStructure<ardupilot::equipment::power::BatteryInfoAux> &)>
        BatteryInfoAuxCbBinder;

    uavcan::Subscriber<uavcan::equipment::power::BatteryInfo, BatteryInfoCbBinder>          _sub_battery;
    uavcan::Subscriber<ardupilot::equipment::power::BatteryInfoAux, BatteryInfoAuxCbBinder> _sub_battery_aux;

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::BAT_LOW_THR>)_param_bat_low_thr,
        (ParamFloat<params_id::BAT_CRIT_THR>)_param_bat_crit_thr,
        (ParamFloat<params_id::BAT_EMERGEN_THR>)_param_bat_emergen_thr)

    float            _discharged_mah      = 0.f;
    float            _discharged_mah_loop = 0.f;
    uint8_t          _warning;
    hrt_abstime      _last_timestamp;
    battery_status_s _battery_status[battery_status_s::MAX_INSTANCES]{};
    BatteryDataType  _batt_update_mod[battery_status_s::MAX_INSTANCES]{};

    static constexpr int FILTER_DATA        = 2;
    static constexpr int BATTERY_INDEX_1    = 1;
    static constexpr int BATTERY_INDEX_2    = 2;
    static constexpr int BATTERY_INDEX_3    = 3;
    static constexpr int BATTERY_INDEX_4    = 4;
    static constexpr int SAMPLE_INTERVAL_US = 20_ms; // assume higher frequency UAVCAN feedback than 50Hz

    static_assert(battery_status_s::MAX_INSTANCES <= BATTERY_INDEX_4, "Battery array too big");

    Battery battery1 = {BATTERY_INDEX_1, this, SAMPLE_INTERVAL_US, battery_status_s::BATTERY_SOURCE_EXTERNAL};
    Battery battery2 = {BATTERY_INDEX_2, this, SAMPLE_INTERVAL_US, battery_status_s::BATTERY_SOURCE_EXTERNAL};
    Battery battery3 = {BATTERY_INDEX_3, this, SAMPLE_INTERVAL_US, battery_status_s::BATTERY_SOURCE_EXTERNAL};
    Battery battery4 = {BATTERY_INDEX_4, this, SAMPLE_INTERVAL_US, battery_status_s::BATTERY_SOURCE_EXTERNAL};

    Battery *_battery[battery_status_s::MAX_INSTANCES] = {&battery1, &battery2, &battery3, &battery4};
};
