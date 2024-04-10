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
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/differential_pressure.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_local_position.h>

using namespace time_literals;

static constexpr float TEMPERATURE_MSL = 288.15;   // temperature at MSL [K] (15 [C])
static constexpr float PRESSURE_MSL    = 101325.0; // pressure at MSL [Pa]
static constexpr float LAPSE_RATE      = 0.0065;   // reduction in temperature with altitude for troposphere [K/m]
static constexpr float AIR_DENSITY_MSL = 1.225;    // air density at MSL [kg/m^3]

class SensorAirspeedSim : public ModuleCommand<SensorAirspeedSim>, public ModuleParams, public WorkItemScheduled {
public:
    SensorAirspeedSim();
    ~SensorAirspeedSim() override;

    /** @see ModuleCommand */
    static int *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;

    // generate white Gaussian noise sample with std=1
    static float generate_wgn();

    // generate white Gaussian noise sample as a 3D vector with specified std
    matrix::Vector3f noiseGauss3f(float stdx, float stdy, float stdz) {
        return matrix::Vector3f(generate_wgn() * stdx, generate_wgn() * stdy, generate_wgn() * stdz);
    }

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::Subscription         _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription         _vehicle_global_position_sub{ORB_ID(vehicle_global_position_groundtruth)};
    uORB::Subscription         _vehicle_local_position_sub{ORB_ID(vehicle_local_position_groundtruth)};

    uORB::PublicationMulti<differential_pressure_s> _differential_pressure_pub{ORB_ID(differential_pressure)};

    perf_counter_t _loop_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    DEFINE_PARAMETERS(
        (ParamInt<params_id::SIM_ARSPD_FAIL>)_sim_failure)
};
