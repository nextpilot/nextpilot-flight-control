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

#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>

#include <workq/WorkItemScheduled.hpp>
#include <hrtimer.h>
#include <mathlib/math/WelfordMeanVector.hpp>
#include <perf/perf_counter.h>
#include <sensor_calibration/Gyroscope.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/vehicle_status.h>

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::global_params;

class GyroCalibration : public ModuleCommand<GyroCalibration>, public ModuleParams, public WorkItemScheduled {
public:
    GyroCalibration();
    ~GyroCalibration() override;

    /** @see ModuleCommand */
    static GyroCalibration *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

    int print_status() override;

private:
    static constexpr hrt_abstime INTERVAL_US = 20000_us;
    static constexpr int         MAX_SENSORS = 4;

    void Run() override;

    void Reset() {
        for (int gyro = 0; gyro < MAX_SENSORS; gyro++) {
            _gyro_mean[gyro].reset();
            _gyro_last_update[gyro] = 0;
        }

        _last_calibration_update = hrt_absolute_time();
    }

    // return the square of two floating point numbers
    static constexpr float sq(float var) {
        return var * var;
    }

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::Subscription         _vehicle_status_sub{ORB_ID::vehicle_status};

    uORB::SubscriptionMultiArray<sensor_accel_s, MAX_SENSORS> _sensor_accel_subs{ORB_ID::sensor_accel};
    uORB::SubscriptionMultiArray<sensor_gyro_s, MAX_SENSORS>  _sensor_gyro_subs{ORB_ID::sensor_gyro};

    calibration::Gyroscope            _gyro_calibration[MAX_SENSORS]{};
    math::WelfordMeanVector<float, 3> _gyro_mean[MAX_SENSORS]{};
    matrix::Vector3f                  _gyro_cal_variance[MAX_SENSORS]{};
    float                             _temperature[MAX_SENSORS]{};
    hrt_abstime                       _gyro_last_update[MAX_SENSORS]{};

    matrix::Vector3f _acceleration[MAX_SENSORS]{};

    hrt_abstime _last_calibration_update{0};

    bool _armed{false};
    bool _system_calibrating{false};

    perf_counter_t _loop_interval_perf{perf_alloc(PC_INTERVAL, MODULE_NAME ": interval")};
    perf_counter_t _calibration_updated_perf{perf_alloc(PC_COUNT, MODULE_NAME ": calibration updated")};
};
