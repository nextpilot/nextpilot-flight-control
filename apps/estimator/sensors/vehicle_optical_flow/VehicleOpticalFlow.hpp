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
#include "RingBuffer.hpp"

#include <Integrator.hpp>

#include <mathlib/math/Limits.hpp>
#include <matrix/matrix/math.hpp>
#include <perf/perf_counter.h>
#include <sensor_calibration/Gyroscope.hpp>
#include <ulog/log.h>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/distance_sensor.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/sensor_optical_flow.h>
#include <uORB/topics/sensor_selection.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_optical_flow.h>
#include <uORB/topics/vehicle_optical_flow_vel.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

namespace sensors {

class VehicleOpticalFlow : public ModuleParams, public WorkItemScheduled {
public:
    VehicleOpticalFlow();
    ~VehicleOpticalFlow() override;

    bool Start();
    void Stop();

    void PrintStatus();

private:
    void ClearAccumulatedData();
    void UpdateDistanceSensor();
    void UpdateSensorGyro();

    void Run() override;

    void ParametersUpdate();
    void SensorCorrectionsUpdate(bool force = false);

    static constexpr int MAX_SENSOR_COUNT = 3;

    uORB::Publication<vehicle_optical_flow_s>     _vehicle_optical_flow_pub{ORB_ID(vehicle_optical_flow)};
    uORB::Publication<vehicle_optical_flow_vel_s> _vehicle_optical_flow_vel_pub{ORB_ID(vehicle_optical_flow_vel)};

    uORB::Subscription _params_sub{ORB_ID(parameter_update)};

    uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_subs{ORB_ID::distance_sensor};

    uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};

    uORB::SubscriptionCallbackWorkItem _sensor_flow_sub{this, ORB_ID(sensor_optical_flow)};
    uORB::SubscriptionCallbackWorkItem _sensor_gyro_sub{this, ORB_ID(sensor_gyro)};
    uORB::SubscriptionCallbackWorkItem _sensor_selection_sub{this, ORB_ID(sensor_selection)};

    sensors::IntegratorConing _gyro_integrator{};

    hrt_abstime _gyro_timestamp_sample_last{0};

    calibration::Gyroscope _gyro_calibration{};

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    matrix::Dcmf _flow_rotation{matrix::eye<float, 3>()};

    hrt_abstime      _flow_timestamp_sample_last{0};
    matrix::Vector2f _flow_integral{};
    matrix::Vector3f _delta_angle{};
    uint32_t         _integration_timespan_us{};
    float            _distance_sum{NAN};
    uint8_t          _distance_sum_count{0};
    uint16_t         _quality_sum{0};
    uint8_t          _accumulated_count{0};

    int         _distance_sensor_selected{-1}; // because we can have several distance sensor instances with different orientations
    hrt_abstime _last_range_sensor_update{0};

    bool _delta_angle_available{false};

    struct gyroSample {
        uint64_t         time_us{}; ///< timestamp of the measurement (uSec)
        matrix::Vector3f data{};
        float            dt{0.f};
    };

    struct rangeSample {
        uint64_t time_us{}; ///< timestamp of the measurement (uSec)
        float    data{};
    };

    RingBuffer<gyroSample, 32> _gyro_buffer{};
    RingBuffer<rangeSample, 5> _range_buffer{};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::SENS_FLOW_ROT>)_param_sens_flow_rot,
        (ParamFloat<params_id::SENS_FLOW_MINHGT>)_param_sens_flow_minhgt,
        (ParamFloat<params_id::SENS_FLOW_MAXHGT>)_param_sens_flow_maxhgt,
        (ParamFloat<params_id::SENS_FLOW_MAXR>)_param_sens_flow_maxr,
        (ParamFloat<params_id::SENS_FLOW_RATE>)_param_sens_flow_rate)
};
}; // namespace sensors
