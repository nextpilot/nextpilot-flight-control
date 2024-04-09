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

#include <drivers/drv_hrt.h>
#include <lib/mathlib/mathlib.h>
#include <lib/parameters/param.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/atomic.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.hpp>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/px4_work_queue/WorkItemScheduled.hpp>
#include <px4_platform_common/tasks.h>
#include <px4_platform_common/time.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionInterval.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/sensor_correction.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>

#include "TemperatureCompensation.h"

using namespace time_literals;

namespace temperature_compensation {

class TemperatureCompensationModule : public ModuleCommand<TemperatureCompensationModule>, public ModuleParams, public WorkItemScheduled {
public:
    TemperatureCompensationModule();
    ~TemperatureCompensationModule() override;

    /** @see ModuleCommand */
    static int *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static TemperatureCompensationModule *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    /**
     * Initializes scheduling on work queue.
     */
    int init() override;

private:
    void Run() override;

    void accelPoll();
    void gyroPoll();
    void baroPoll();

    /**
     * call this whenever parameters got updated. Make sure to have initialize_sensors() called at least
     * once before calling this.
     */
    void parameters_update();

    uORB::Subscription _accel_subs[ACCEL_COUNT_MAX]{
        {ORB_ID(sensor_accel), 0},
        {ORB_ID(sensor_accel), 1},
        {ORB_ID(sensor_accel), 2},
        {ORB_ID(sensor_accel), 3},
    };

    uORB::Subscription _gyro_subs[GYRO_COUNT_MAX]{
        {ORB_ID(sensor_gyro), 0},
        {ORB_ID(sensor_gyro), 1},
        {ORB_ID(sensor_gyro), 2},
        {ORB_ID(sensor_gyro), 3},
    };

    uORB::Subscription _baro_subs[BARO_COUNT_MAX]{
        {ORB_ID(sensor_baro), 0},
        {ORB_ID(sensor_baro), 1},
        {ORB_ID(sensor_baro), 2},
        {ORB_ID(sensor_baro), 3},
    };

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _vehicle_command_sub{ORB_ID(vehicle_command)};

    perf_counter_t _loop_perf; /**< loop performance counter */

    orb_advert_t _mavlink_log_pub{nullptr};

    /* sensor thermal compensation */
    TemperatureCompensation _temperature_compensation;

    sensor_correction_s                    _corrections{}; /**< struct containing the sensor corrections to be published to the uORB*/
    uORB::Publication<sensor_correction_s> _sensor_correction_pub{ORB_ID(sensor_correction)};

    bool _corrections_changed{true};
};

} // namespace temperature_compensation
