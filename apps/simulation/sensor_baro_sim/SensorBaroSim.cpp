/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "sim_baro"

#include "SensorBaroSim.hpp"
#include <device/device_id.h>

using namespace matrix;

SensorBaroSim::SensorBaroSim() :
    ModuleParams(nullptr),
    WorkItemScheduled(MODULE_NAME, wq_configurations::hp_default) {
    srand(1234); // initialize the random seed once before calling generate_wgn()
}

SensorBaroSim::~SensorBaroSim() {
    perf_free(_loop_perf);
}

int SensorBaroSim::init() {
    ScheduleOnInterval(50_ms); // 20 Hz
    return 0;
}

float SensorBaroSim::generate_wgn() {
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

void SensorBaroSim::Run() {
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
            const float dt = math::constrain((gpos.timestamp - _last_update_time) * 1e-6f, 0.001f, 0.1f);

            const float alt_msl = gpos.alt;

            // calculate abs_pressure using an ISA model for the tropsphere (valid up to 11km above MSL)
            const float lapse_rate      = 0.0065f; // reduction in temperature with altitude (Kelvin/m)
            const float temperature_msl = 288.0f;  // temperature at MSL (Kelvin)

            const float temperature_local = temperature_msl - lapse_rate * alt_msl;
            const float pressure_ratio    = powf(temperature_msl / temperature_local, 5.256f);
            const float pressure_msl      = 101325.0f; // pressure at MSL
            const float absolute_pressure = pressure_msl / pressure_ratio;

            // generate Gaussian noise sequence using polar form of Box-Muller transformation
            double y1;
            {
                double x1;
                double x2;
                double w;

                if (!_baro_rnd_use_last) {
                    do {
                        x1 = 2. * (double)generate_wgn() - 1.;
                        x2 = 2. * (double)generate_wgn() - 1.;
                        w  = x1 * x1 + x2 * x2;
                    } while (w >= 1.0);

                    w = sqrt((-2.0 * log(w)) / w);
                    // calculate two values - the second value can be used next time because it is uncorrelated
                    y1                 = x1 * w;
                    _baro_rnd_y2       = x2 * w;
                    _baro_rnd_use_last = true;

                } else {
                    // no need to repeat the calculation - use the second value from last update
                    y1                 = _baro_rnd_y2;
                    _baro_rnd_use_last = false;
                }
            }

            // Apply noise and drift
            const float abs_pressure_noise = 1.f * (float)y1; // 1 Pa RMS noise
            _baro_drift_pa += _baro_drift_pa_per_sec * dt;
            const float absolute_pressure_noisy = absolute_pressure + abs_pressure_noise + _baro_drift_pa;

            // convert to hPa
            float pressure = absolute_pressure_noisy + _sim_baro_off_p.get();

            // calculate temperature in Celsius
            float temperature = temperature_local - 273.0f + _sim_baro_off_t.get();

            // publish
            sensor_baro_s sensor_baro{};
            sensor_baro.timestamp_sample = gpos.timestamp;
            sensor_baro.device_id        = 6620172; // 6620172: DRV_BARO_DEVTYPE_BAROSIM, BUS: 1, ADDR: 4, TYPE: SIMULATION
            sensor_baro.pressure         = pressure;
            sensor_baro.temperature      = temperature;
            sensor_baro.timestamp        = hrt_absolute_time();
            _sensor_baro_pub.publish(sensor_baro);

            _last_update_time = gpos.timestamp;
        }
    }

    perf_end(_loop_perf);
}

SensorBaroSim *SensorBaroSim::instantiate(int argc, char *argv[]) {
    SensorBaroSim *instance = new SensorBaroSim();

    // if (instance) {
    //     _object.store(instance);
    //     _task_id = task_id_is_work_queue;

    //     if (instance->init()) {
    //         return PX4_OK;
    //     }

    // } else {
    //     PX4_ERR("alloc failed");
    // }

    // delete instance;
    // _object.store(nullptr);
    // _task_id = -1;

    // return PX4_ERROR;

    return instance;
}

int SensorBaroSim::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int SensorBaroSim::print_usage(const char *reason) {
    if (reason) {
        PX4_WARN("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description


)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("sensor_baro_sim", "system");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

extern "C" __EXPORT int sensor_baro_sim_main(int argc, char *argv[]) {
    return SensorBaroSim::main(argc, argv);
}
MSH_CMD_EXPORT_ALIAS(sensor_baro_sim_main, sim_baro, baro simulator);

int sensor_baro_sim_start() {
    int32_t hitl = param_get_int32((param_t)params_id::SYS_HITL);

    if (true /*hitl == 2*/) {
        const char *argv[] = {"sim_baro", "start"};
        int         argc   = sizeof(argv) / sizeof(argv[0]);
        return SensorBaroSim::main(argc, (char **)argv);
    }

    return 0;
}
INIT_APP_EXPORT(sensor_baro_sim_start);
