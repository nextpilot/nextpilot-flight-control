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

#include <hrtimer.h>
#include <field_sensor_bias_estimator/FieldSensorBiasEstimator.hpp>
#include <mathlib/mathlib.h>
#include <perf/perf_counter.h>
#include <sensor_calibration/Magnetometer.hpp>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/magnetometer_bias_estimate.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_mag.h>
#include <uORB/topics/vehicle_angular_velocity.h>
#include <uORB/topics/vehicle_status.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

namespace mag_bias_estimator {

class MagBiasEstimator : public ModuleCommand<MagBiasEstimator>, public ModuleParams, public WorkItemScheduled {
public:
    MagBiasEstimator();
    ~MagBiasEstimator() override;

    static MagBiasEstimator *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]) {
        return print_usage("unknown command");
    }

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    void start();

private:
    void Run() override;
    void publishMagBiasEstimate();

    static constexpr int MAX_SENSOR_COUNT = 4;

    FieldSensorBiasEstimator _bias_estimator[MAX_SENSOR_COUNT];
    hrt_abstime              _timestamp_last_update[MAX_SENSOR_COUNT]{};

    uORB::SubscriptionMultiArray<sensor_mag_s, MAX_SENSOR_COUNT> _sensor_mag_subs{ORB_ID::sensor_mag};
    uORB::Subscription                                           _parameter_update_sub{ORB_ID(parameter_update)};
    uORB::Subscription                                           _vehicle_angular_velocity_sub{ORB_ID(vehicle_angular_velocity)};
    uORB::Subscription                                           _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::Publication<magnetometer_bias_estimate_s> _magnetometer_bias_estimate_pub{ORB_ID(magnetometer_bias_estimate)};

    calibration::Magnetometer _calibration[MAX_SENSOR_COUNT];

    hrt_abstime _time_valid[MAX_SENSOR_COUNT]{};

    bool _reset_field_estimator[MAX_SENSOR_COUNT]{};
    bool _valid[MAX_SENSOR_COUNT]{};

    uint8_t _arming_state{0};
    bool    _system_calibrating{false};

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::MBE_LEARN_GAIN>)_param_mbe_learn_gain)
};

} // namespace mag_bias_estimator
