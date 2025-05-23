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

#include <perf/perf_counter.h>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/vehicle_global_position.h>
#include <matrix/math.hpp>
#include <mathlib/mathlib.h>

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::workq;
using namespace nextpilot::param;

class SensorBaroSim : public ModuleCommand<SensorBaroSim>, public ModuleParams, public WorkItemScheduled {
public:
    SensorBaroSim();
    ~SensorBaroSim() override;

    /** @see ModuleCommand */
    static SensorBaroSim *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;

    // generate white Gaussian noise sample with std=1
    static float generate_wgn();

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::Subscription         _vehicle_global_position_sub{ORB_ID(vehicle_global_position_groundtruth)};

    bool   _baro_rnd_use_last{false};
    double _baro_rnd_y2{0.0};
    float  _baro_drift_pa_per_sec{0.0};
    float  _baro_drift_pa{0.0};

    hrt_abstime _last_update_time{0};

    uORB::PublicationMulti<sensor_baro_s> _sensor_baro_pub{ORB_ID(sensor_baro)};

    perf_counter_t _loop_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::SIM_BARO_OFF_P>)_sim_baro_off_p,
        (ParamFloat<params_id::SIM_BARO_OFF_T>)_sim_baro_off_t)
};
