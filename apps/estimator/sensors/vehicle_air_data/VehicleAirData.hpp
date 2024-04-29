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

#include "data_validator/DataValidatorGroup.hpp"

#include <sensor_calibration/Barometer.hpp>
#include <mathlib/math/Limits.hpp>
#include <matrix/matrix/math.hpp>
#include <perf/perf_counter.h>
#include <ulog/mavlink_log.h>
#include <ulog/log.h>
#include <module/module_params.hpp>
// #include <px4_platform_common/px4_config.h>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/differential_pressure.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/sensors_status.h>
#include <uORB/topics/vehicle_air_data.h>

using namespace time_literals;

namespace sensors {
class VehicleAirData : public ModuleParams, public WorkItemScheduled {
public:
    VehicleAirData();
    ~VehicleAirData() override;

    bool Start();
    void Stop();

    void PrintStatus();

private:
    void Run() override;

    void AirTemperatureUpdate();
    void CheckFailover(const hrt_abstime &time_now_us);
    bool ParametersUpdate(bool force = false);
    void UpdateStatus();

    float PressureToAltitude(float pressure_pa, float temperature = 15.f) const;

    static constexpr int MAX_SENSOR_COUNT = 4;

    uORB::Publication<sensors_status_s> _sensors_status_baro_pub{ORB_ID(sensors_status_baro)};

    uORB::Publication<vehicle_air_data_s> _vehicle_air_data_pub{ORB_ID(vehicle_air_data)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _differential_pressure_sub{ORB_ID(differential_pressure)};

    uORB::SubscriptionCallbackWorkItem _sensor_sub[MAX_SENSOR_COUNT]{
        {this, ORB_ID(sensor_baro), 0},
        {this, ORB_ID(sensor_baro), 1},
        {this, ORB_ID(sensor_baro), 2},
        {this, ORB_ID(sensor_baro), 3},
    };

    calibration::Barometer _calibration[MAX_SENSOR_COUNT];

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    hrt_abstime  _last_error_message{0};
    orb_advert_t _mavlink_log_pub{nullptr};

    DataValidatorGroup _voter{1};
    unsigned           _last_failover_count{0};

    uint64_t    _timestamp_sample_sum[MAX_SENSOR_COUNT]{0};
    float       _data_sum[MAX_SENSOR_COUNT]{};
    float       _temperature_sum[MAX_SENSOR_COUNT]{};
    int         _data_sum_count[MAX_SENSOR_COUNT]{};
    hrt_abstime _last_publication_timestamp[MAX_SENSOR_COUNT]{};

    float _last_data[MAX_SENSOR_COUNT]{};
    bool  _advertised[MAX_SENSOR_COUNT]{};

    float _sensor_diff[MAX_SENSOR_COUNT]{}; // filtered differences between sensor instances

    uint8_t _priority[MAX_SENSOR_COUNT]{};

    int8_t _selected_sensor_sub_index{-1};

    float _air_temperature_celsius{20.f}; // initialize with typical 20degC ambient temperature

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::SENS_BARO_QNH>)_param_sens_baro_qnh,
        (ParamFloat<params_id::SENS_BARO_RATE>)_param_sens_baro_rate)
};
}; // namespace sensors
