/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "SensorMagSim.hpp"

#include <drivers/drv_sensor.h>
#include <world_magnetic_model/geo_mag_declination.h>

using namespace matrix;

SensorMagSim::SensorMagSim() :
    ModuleParams(nullptr),
    WorkItemScheduled(MODULE_NAME, wq_configurations::hp_default) {
    _px4_mag.set_device_type(DRV_MAG_DEVTYPE_MAGSIM);
}

SensorMagSim::~SensorMagSim() {
    perf_free(_loop_perf);
}

bool SensorMagSim::init() {
    ScheduleOnInterval(20_ms); // 50 Hz
    return true;
}

float SensorMagSim::generate_wgn() {
    // generate white Gaussian noise sample with std=1

    // algorithm 1:
    // float temp=((float)(rand()+1))/(((float)RAND_MAX+1.0f));
    // return sqrtf(-2.0f*logf(temp))*cosf(2.0f*M_PI_F*rand()/RAND_MAX);
    // algorithm 2: from BlockRandGauss.hpp
    static float V1, V2, S;
    static bool  phase = true;
    float        X;

    if (phase) {
        do {
            float U1 = (float)rand() / (float)RAND_MAX;
            float U2 = (float)rand() / (float)RAND_MAX;
            V1       = 2.0f * U1 - 1.0f;
            V2       = 2.0f * U2 - 1.0f;
            S        = V1 * V1 + V2 * V2;
        } while (S >= 1.0f || fabsf(S) < 1e-8f);

        X = V1 * float(sqrtf(-2.0f * float(logf(S)) / S));

    } else {
        X = V2 * float(sqrtf(-2.0f * float(logf(S)) / S));
    }

    phase = !phase;
    return X;
}

void SensorMagSim::Run() {
    if (should_exit()) {
        ScheduleClear();
        exit_and_cleanup();
        return;
    }

    perf_begin(_loop_perf);

    // Check if parameters have changed
    if (_parameter_update_sub.updated()) {
        // clear update
        parameter_update_s param_update;
        _parameter_update_sub.copy(&param_update);

        updateParams();
    }

    if (_vehicle_global_position_sub.updated()) {
        vehicle_global_position_s gpos;

        if (_vehicle_global_position_sub.copy(&gpos)) {
            if (gpos.eph < 1000) {
                // magnetic field data returned by the geo library using the current GPS position
                const float mag_declination_gps = get_mag_declination_radians(gpos.lat, gpos.lon);
                const float mag_inclination_gps = get_mag_inclination_radians(gpos.lat, gpos.lon);
                const float mag_strength_gps    = get_mag_strength_gauss(gpos.lat, gpos.lon);

                _mag_earth_pred = Dcmf(Eulerf(0, -mag_inclination_gps, mag_declination_gps)) * Vector3f(mag_strength_gps, 0, 0);

                _mag_earth_available = true;
            }
        }
    }

    if (_mag_earth_available) {
        vehicle_attitude_s attitude;

        if (_vehicle_attitude_sub.update(&attitude)) {
            Vector3f expected_field = Dcmf{Quatf{attitude.q}}.transpose() * _mag_earth_pred;

            expected_field += noiseGauss3f(0.02f, 0.02f, 0.03f);

            _px4_mag.update(attitude.timestamp,
                            expected_field(0) + _sim_mag_offset_x.get(),
                            expected_field(1) + _sim_mag_offset_y.get(),
                            expected_field(2) + _sim_mag_offset_z.get());
        }
    }

    perf_end(_loop_perf);
}

int SensorMagSim::instantiate(int argc, char *argv[]) {
    SensorMagSim *instance = new SensorMagSim();

    if (instance) {
        _object.store(instance);
        _task_id = task_id_is_work_queue;

        if (instance->init()) {
            return PX4_OK;
        }

    } else {
        PX4_ERR("alloc failed");
    }

    delete instance;
    _object.store(nullptr);
    _task_id = -1;

    return PX4_ERROR;
}

int SensorMagSim::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int SensorMagSim::print_usage(const char *reason) {
    if (reason) {
        PX4_WARN("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description


)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("sensor_mag_sim", "system");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

extern "C" __EXPORT int sensor_mag_sim_main(int argc, char *argv[]) {
    return SensorMagSim::main(argc, argv);
}
