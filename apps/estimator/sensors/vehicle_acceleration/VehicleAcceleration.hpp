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

#include <sensor_calibration/Accelerometer.hpp>
#include <mathlib/math/Limits.hpp>
#include <matrix/matrix/math.hpp>
#include <mathlib/math/filter/LowPassFilter2p.hpp>
#include <rtdbg.h>
#include <module/module_params.hpp>
// #include <px4_platform_common/px4_config.h>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/estimator_selector_status.h>
#include <uORB/topics/estimator_sensor_bias.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/sensor_selection.h>
#include <uORB/topics/vehicle_acceleration.h>

using namespace time_literals;

namespace sensors {

class VehicleAcceleration : public ModuleParams, public WorkItemScheduled {
public:
    VehicleAcceleration();
    ~VehicleAcceleration() override;

    bool Start();
    void Stop();

    void PrintStatus();

private:
    void Run() override;

    void CheckAndUpdateFilters();
    void ParametersUpdate(bool force = false);
    void SensorBiasUpdate(bool force = false);
    bool SensorSelectionUpdate(bool force = false);

    static constexpr int MAX_SENSOR_COUNT = 4;

    uORB::Publication<vehicle_acceleration_s> _vehicle_acceleration_pub{ORB_ID(vehicle_acceleration)};

    uORB::Subscription _estimator_selector_status_sub{ORB_ID(estimator_selector_status)};
    uORB::Subscription _estimator_sensor_bias_sub{ORB_ID(estimator_sensor_bias)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::SubscriptionCallbackWorkItem _sensor_selection_sub{this, ORB_ID(sensor_selection)};
    uORB::SubscriptionCallbackWorkItem _sensor_sub{this, ORB_ID(sensor_accel)};

    calibration::Accelerometer _calibration{};

    matrix::Vector3f _bias{};

    matrix::Vector3f _acceleration_prev{};

    float _filter_sample_rate{NAN};

    math::LowPassFilter2p<matrix::Vector3f> _lp_filter{};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::IMU_ACCEL_CUTOFF>)_param_imu_accel_cutoff,
        (ParamInt<params_id::IMU_INTEG_RATE>)_param_imu_integ_rate)
};

} // namespace sensors
