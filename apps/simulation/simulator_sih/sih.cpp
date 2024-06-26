/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file sih.cpp
 * Simulator in Hardware
 *
 * @author Romain Chiappinelli      <romain.chiap@gmail.com>
 *
 * Coriolis g Corporation - January 2019
 */

#define LOG_TAG "sih"
#define LOG_LVL LOG_LVL_INFO

#include "aero.hpp"
#include "sih.hpp"
#include <getopt/getopt.h>
#include <ulog/log.h>
#include <drivers/drv_pwm_output.h> // to get PWM flags
#include <device/device_id.h>

using namespace math;
using namespace matrix;
using namespace time_literals;

// #define SIH_DEBUG

Sih::Sih() :
    ModuleParams(nullptr),
    ModuleThread(LOG_TAG, 2048, 16, 10) {
}

Sih::~Sih() {
    perf_free(_loop_perf);
    perf_free(_loop_interval_perf);
}

void Sih::Run() {
    _px4_accel.set_temperature(T1_C);
    _px4_gyro.set_temperature(T1_C);

    parameters_updated();
    init_variables();

    const hrt_abstime task_start = hrt_absolute_time();
    _last_run                    = task_start;
    _airspeed_time               = task_start;
    _dist_snsr_time              = task_start;
    _vehicle                     = (VehicleType)constrain(_sih_vtype.get(), static_cast<typeof _sih_vtype.get()>(0),
                                                          static_cast<typeof _sih_vtype.get()>(3));

    _actuator_out_sub = uORB::Subscription{ORB_ID(actuator_outputs_sim)};

#if defined(ENABLE_LOCKSTEP_SCHEDULER)
    lockstep_loop();
#else
    realtime_loop();
#endif
    exit_and_cleanup();
}

#if defined(ENABLE_LOCKSTEP_SCHEDULER)
// Get current timestamp in microseconds
static uint64_t micros() {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec * ((uint64_t)1000000) + t.tv_usec;
}

void Sih::lockstep_loop() {
    int rate = math::min(_imu_gyro_ratemax.get(), _imu_integration_rate.get());

    // default to 400Hz (2500 us interval)
    if (rate <= 0) {
        rate = 400;
    }

    // 200 - 2000 Hz
    int sim_interval_us = math::constrain(int(roundf(1e6f / rate)), 500, 5000);

    float       speed_factor = 1.f;
    const char *speedup      = getenv("PX4_SIM_SPEED_FACTOR");

    if (speedup) {
        speed_factor = atof(speedup);
    }

    int rt_interval_us = int(roundf(sim_interval_us / speed_factor));

    rt_kprintf("Simulation loop with %d Hz (%d us sim time interval\n)", rate, sim_interval_us);
    rt_kprintf("Simulation with %.1fx speedup. Loop with (%d us wall time interval\n)", (double)speed_factor, rt_interval_us);
    uint64_t pre_compute_wall_time_us;

    while (!should_exit()) {
        pre_compute_wall_time_us = micros();
        perf_count(_loop_interval_perf);

        _current_simulation_time_us += sim_interval_us;
        struct timespec ts;
        abstime_to_ts(&ts, _current_simulation_time_us);
        clock_settime(CLOCK_MONOTONIC, &ts);

        perf_begin(_loop_perf);
        sensor_step();
        perf_end(_loop_perf);

        // Only do lock-step once we received the first actuator output
        int      sleep_time;
        uint64_t current_wall_time_us;

        if (_last_actuator_output_time <= 0) {
            PX4_DEBUG("SIH starting up - no lockstep yet");
            current_wall_time_us = micros();
            sleep_time           = math::max(0, sim_interval_us - (int)(current_wall_time_us - pre_compute_wall_time_us));

        } else {
            // px4_lockstep_wait_for_components();
            current_wall_time_us = micros();
            sleep_time           = math::max(0, rt_interval_us - (int)(current_wall_time_us - pre_compute_wall_time_us));
        }

        _achieved_speedup = 0.99f * _achieved_speedup + 0.01f * ((float)sim_interval_us / (float)(current_wall_time_us - pre_compute_wall_time_us + sleep_time));
        usleep(sleep_time);
    }
}
#endif

static void timer_callback(void *sem) {
    rt_sem_release((rt_sem_t)sem);
}

void Sih::realtime_loop() {
    int rate = _imu_gyro_ratemax.get();

    // default to 250 Hz (4000 us interval)
    if (rate <= 0) {
        rate = 250;
    }

    // 200 - 2000 Hz
    int interval_us = math::constrain(int(roundf(1e6f / rate)), 500, 5000);

    rt_sem_init(&_data_semaphore, "sih_lock", 0, RT_IPC_FLAG_PRIO);
    hrt_call_every(&_timer_call, interval_us, interval_us, timer_callback, &_data_semaphore);

    while (!should_exit()) {
        rt_sem_take(&_data_semaphore, RT_WAITING_FOREVER); // periodic real time wakeup
#ifdef SIH_DEBUG
        static hrt_abstime last_us = hrt_absolute_time();
        float              dt_ms   = (hrt_absolute_time() - last_us) * 1e-3f;
        last_us                    = hrt_absolute_time();
        LOG_I("sih dt: %.3fms", dt_ms);
#endif // SIH_DEBUG
        perf_begin(_loop_perf);
        sensor_step();
        perf_end(_loop_perf);
    }

    hrt_cancel(&_timer_call);
    rt_sem_detach(&_data_semaphore);
}

void Sih::sensor_step() {
    // check for parameter updates
    if (_parameter_update_sub.updated()) {
        // clear update
        parameter_update_s pupdate;
        _parameter_update_sub.copy(&pupdate);

        // update parameters from storage
        updateParams();
        parameters_updated();
    }

    perf_begin(_loop_perf);

    const hrt_abstime now = hrt_absolute_time();
    const float       dt  = (now - _last_run) * 1e-6f;
    _last_run             = now;

    read_motors(dt);

    generate_force_and_torques();

    equations_of_motion(dt);

    reconstruct_sensors_signals(now);

    if ((_vehicle == VehicleType::FW || _vehicle == VehicleType::TS) && now - _airspeed_time >= 50_ms) {
        _airspeed_time = now;
        send_airspeed(now);
    }

    // distance sensor published at 50 Hz
    if (now - _dist_snsr_time >= 20_ms && fabs(_distance_snsr_override) < 10000) {
        _dist_snsr_time = now;
        send_dist_snsr(now);
    }

    publish_ground_truth(now);

    perf_end(_loop_perf);
}

void Sih::parameters_updated() {
    _T_MAX   = _sih_t_max.get();
    _Q_MAX   = _sih_q_max.get();
    _L_ROLL  = _sih_l_roll.get();
    _L_PITCH = _sih_l_pitch.get();
    _KDV     = _sih_kdv.get();
    _KDW     = _sih_kdw.get();
    _H0      = _sih_h0.get();

    _LAT0     = (double)_sih_lat0.get() * 1.0e-7;
    _LON0     = (double)_sih_lon0.get() * 1.0e-7;
    _COS_LAT0 = cosl((long double)radians(_LAT0));

    _MASS = _sih_mass.get();

    _W_I = Vector3f(0.0f, 0.0f, _MASS * CONSTANTS_ONE_G);

    _I       = diag(Vector3f(_sih_ixx.get(), _sih_iyy.get(), _sih_izz.get()));
    _I(0, 1) = _I(1, 0) = _sih_ixy.get();
    _I(0, 2) = _I(2, 0) = _sih_ixz.get();
    _I(1, 2) = _I(2, 1) = _sih_iyz.get();

    // guards against too small determinants
    _Im1 = 100.0f * inv(static_cast<typeof _I>(100.0f * _I));

    _distance_snsr_min      = _sih_distance_snsr_min.get();
    _distance_snsr_max      = _sih_distance_snsr_max.get();
    _distance_snsr_override = _sih_distance_snsr_override.get();

    _T_TAU = _sih_thrust_tau.get();
}

void Sih::init_variables() {
    srand(1234); // initialize the random seed once before calling generate_wgn()

    _p_I = Vector3f(0.0f, 0.0f, 0.0f);
    _v_I = Vector3f(0.0f, 0.0f, 0.0f);
    _q   = Quatf(1.0f, 0.0f, 0.0f, 0.0f);
    _w_B = Vector3f(0.0f, 0.0f, 0.0f);

    _u[0] = _u[1] = _u[2] = _u[3] = 0.0f;
}

void Sih::read_motors(const float dt) {
    actuator_outputs_s actuators_out;

    if (_actuator_out_sub.update(&actuators_out)) {
        float u_sp                 = 0.0f;
        _last_actuator_output_time = actuators_out.timestamp;


        for (int i = 0; i < NB_MOTORS; i++) { // saturate the motor signals
            if (_vehicle == VehicleType::MC) {
                u_sp  = actuators_out.output[i];
                _u[i] = _u[i] + dt / _T_TAU * (u_sp - _u[i]); // first order transfer function with time constant tau
            }

            if ((_vehicle == VehicleType::FW && i < 3) || (_vehicle == VehicleType::TS && i > 3)) {
                _u[i] = actuators_out.output[i];
            } else {
                float u_sp = actuators_out.output[i];
                _u[i]      = _u[i] + dt / _T_TAU * (u_sp - _u[i]); // first order transfer function with time constant tau
            }

            // if (_vehicle == VehicleType::VTOL) {
            //     if (i < 8) { // MAIN
            //         u_sp  = math::constrain((actuators_out.output[i] - 1000) / (2000 - 1000), 0.0f, 1.0f);
            //         _u[i] = _u[i] + dt / _T_TAU * (u_sp - _u[i]);
            //     } else if (i < NB_MOTORS) { // Aux
            //         _u[i] = math::constrain(2.0f * (actuators_out.output[i] - 1500) / 1000.0f, -1.0f, 1.0f);
            //         if (i == 10) {
            //             _u[i] = -_u[i];
            //         }
            //     }
            // }
        }
    }
}

void Sih::generate_force_and_torques() {
    if (_vehicle == VehicleType::MC) {
        _T_B  = Vector3f(0.0f, 0.0f, -_T_MAX * (+_u[0] + _u[1] + _u[2] + _u[3]));
        _Mt_B = Vector3f(_L_ROLL * _T_MAX * (-_u[0] + _u[1] + _u[2] - _u[3]),
                         _L_PITCH * _T_MAX * (+_u[0] - _u[1] + _u[2] - _u[3]),
                         _Q_MAX * (+_u[0] + _u[1] - _u[2] - _u[3]));
        _Fa_I = -_KDV * _v_I; // first order drag to slow down the aircraft
        _Ma_B = -_KDW * _w_B; // first order angular damper

    } else if (_vehicle == VehicleType::FW) {
        _T_B = Vector3f(_T_MAX * _u[3], 0.0f, 0.0f); // forward thruster
        // _Mt_B = Vector3f(_Q_MAX*_u[3], 0.0f,0.0f); 	// thruster torque
        _Mt_B = Vector3f();
        generate_fw_aerodynamics();

    } else if (_vehicle == VehicleType::TS) {
        _T_B  = Vector3f(0.0f, 0.0f, -_T_MAX * (_u[0] + _u[1]));
        _Mt_B = Vector3f(_L_ROLL * _T_MAX * (_u[1] - _u[0]), 0.0f, _Q_MAX * (_u[1] - _u[0]));
        generate_ts_aerodynamics();

        // _Fa_I = -_KDV * _v_I;   // first order drag to slow down the aircraft
        // _Ma_B = -_KDW * _w_B;   // first order angular damper
    }
}

void Sih::generate_fw_aerodynamics() {
    _v_B           = _C_IB.transpose() * _v_I; // velocity in body frame [m/s]
    float altitude = _H0 - _p_I(2);
    _wing_l.update_aero(_v_B, _w_B, altitude, _u[0] * FLAP_MAX);
    _wing_r.update_aero(_v_B, _w_B, altitude, -_u[0] * FLAP_MAX);
    _tailplane.update_aero(_v_B, _w_B, altitude, _u[1] * FLAP_MAX, _T_MAX * _u[3]);
    _fin.update_aero(_v_B, _w_B, altitude, _u[2] * FLAP_MAX, _T_MAX * _u[3]);
    _fuselage.update_aero(_v_B, _w_B, altitude);

    // sum of aerodynamic forces
    _Fa_I = _C_IB * (_wing_l.get_Fa() + _wing_r.get_Fa() + _tailplane.get_Fa() + _fin.get_Fa() + _fuselage.get_Fa()) - _KDV * _v_I;

    // aerodynamic moments
    _Ma_B = _wing_l.get_Ma() + _wing_r.get_Ma() + _tailplane.get_Ma() + _fin.get_Ma() + _fuselage.get_Ma() - _KDW * _w_B;
}

void Sih::generate_ts_aerodynamics() {
    // velocity in body frame [m/s]
    _v_B = _C_IB.transpose() * _v_I;

    // the aerodynamic is resolved in a frame like a standard aircraft (nose-right-belly)
    Vector3f v_ts     = _C_BS.transpose() * _v_B;
    Vector3f w_ts     = _C_BS.transpose() * _w_B;
    float    altitude = _H0 - _p_I(2);

    Vector3f Fa_ts{};
    Vector3f Ma_ts{};

    for (int i = 0; i < NB_TS_SEG; i++) {
        if (i <= NB_TS_SEG / 2) {
            _ts[i].update_aero(v_ts, w_ts, altitude, _u[5] * TS_DEF_MAX, _T_MAX * _u[1]);

        } else {
            _ts[i].update_aero(v_ts, w_ts, altitude, -_u[4] * TS_DEF_MAX, _T_MAX * _u[0]);
        }

        Fa_ts += _ts[i].get_Fa();
        Ma_ts += _ts[i].get_Ma();
    }

    _Fa_I = _C_IB * _C_BS * Fa_ts - _KDV * _v_I; // sum of aerodynamic forces
    _Ma_B = _C_BS * Ma_ts - _KDW * _w_B;         // aerodynamic moments
}

void Sih::equations_of_motion(const float dt) {
    _C_IB = matrix::Dcm<float>(_q); // body to inertial transformation

    // Equations of motion of a rigid body
    _p_I_dot = _v_I;                                  // position differential
    _v_I_dot = (_W_I + _Fa_I + _C_IB * _T_B) / _MASS; // conservation of linear momentum
    // _q_dot = _q.derivative1(_w_B);              // attitude differential
    _dq      = Quatf::expq(0.5f * dt * _w_B);
    _w_B_dot = _Im1 * (_Mt_B + _Ma_B - _w_B.cross(_I * _w_B)); // conservation of angular momentum

    // fake ground, avoid free fall
    if (_p_I(2) > 0.0f && (_v_I_dot(2) > 0.0f || _v_I(2) > 0.0f)) {
        if (_vehicle == VehicleType::MC || _vehicle == VehicleType::TS) {
            if (!_grounded) { // if we just hit the floor
                // for the accelerometer, compute the acceleration that will stop the vehicle in one time step
                _v_I_dot = -_v_I / dt;

            } else {
                _v_I_dot.setZero();
            }

            _v_I.setZero();
            _w_B.setZero();
            _grounded = true;

        } else if (_vehicle == VehicleType::FW) {
            if (!_grounded) { // if we just hit the floor
                // for the accelerometer, compute the acceleration that will stop the vehicle in one time step
                _v_I_dot(2) = -_v_I(2) / dt;

            } else {
                // we only allow negative acceleration in order to takeoff
                _v_I_dot(2) = fminf(_v_I_dot(2), 0.0f);
            }

            // integration: Euler forward
            _p_I       = _p_I + _p_I_dot * dt;
            _v_I       = _v_I + _v_I_dot * dt;
            Eulerf RPY = Eulerf(_q);
            RPY(0)     = 0.0f;          // no roll
            RPY(1)     = radians(0.0f); // pitch slightly up if needed to get some lift
            _q         = Quatf(RPY);
            _w_B.setZero();
            _grounded = true;
        }

    } else {
        // integration: Euler forward
        _p_I = _p_I + _p_I_dot * dt;
        _v_I = _v_I + _v_I_dot * dt;
        _q   = _q * _dq;
        _q.normalize();
        // integration Runge-Kutta 4
        // rk4_update(_p_I, _v_I, _q, _w_B);
        _w_B      = constrain(_w_B + _w_B_dot * dt, -6.0f * M_PI_F, 6.0f * M_PI_F);
        _grounded = false;
    }
}

void Sih::reconstruct_sensors_signals(const hrt_abstime &time_now_us) {
    // The sensor signals reconstruction and noise levels are from [1]
    // [1] Bulka, Eitan, and Meyer Nahon. "Autonomous fixed-wing aerobatics: from theory to flight."
    //     In 2018 IEEE International Conference on Robotics and Automation (ICRA), pp. 6573-6580. IEEE, 2018.

    // IMU
    Vector3f acc  = _C_IB.transpose() * (_v_I_dot - Vector3f(0.0f, 0.0f, CONSTANTS_ONE_G)) + noiseGauss3f(0.5f, 1.7f, 1.4f);
    Vector3f gyro = _w_B + noiseGauss3f(0.14f, 0.07f, 0.03f);

    // update IMU every iteration
    _px4_accel.update(time_now_us, acc(0), acc(1), acc(2));
    _px4_gyro.update(time_now_us, gyro(0), gyro(1), gyro(2));

    // publish sensor acceleration
    _sensor_accel.timestamp        = time_now_us;
    _sensor_accel.timestamp_sample = time_now_us;
    _sensor_accel.device_id        = _px4_accel.get_device_id();
    _sensor_accel.x                = acc(0);
    _sensor_accel.y                = acc(1);
    _sensor_accel.z                = acc(2);
    _sensor_accel.temperature      = 25.f;
    _sensor_accel.error_count      = 0;
    for (int acc_axis = 0; acc_axis < 3; acc_axis++) {
        _sensor_accel.clip_counter[acc_axis] = 0;
    }
    _sensor_accel.samples = 1;
    _sensor_accel_pub.publish(_sensor_accel);

    // publish sensor gyroscope
    _sensor_gyro.timestamp        = time_now_us;
    _sensor_gyro.timestamp_sample = time_now_us;
    _sensor_gyro.device_id        = _px4_gyro.get_device_id();
    _sensor_gyro.x                = gyro(0);
    _sensor_gyro.y                = gyro(1);
    _sensor_gyro.z                = gyro(2);
    _sensor_gyro.temperature      = 25.f;
    _sensor_gyro.error_count      = 0;
    for (int gyro_axis = 0; gyro_axis < 3; gyro_axis++) {
        _sensor_gyro.clip_counter[gyro_axis] = 0;
    }
    _sensor_gyro.samples = 1;
    _sensor_gyro_pub.publish(_sensor_gyro);
}

void Sih::send_airspeed(const hrt_abstime &time_now_us) {
    // TODO: send differential pressure instead?
    airspeed_s airspeed{};
    airspeed.timestamp_sample        = time_now_us;
    airspeed.true_airspeed_m_s       = fmaxf(0.1f, _v_B(0) + generate_wgn() * 0.2f);
    airspeed.indicated_airspeed_m_s  = airspeed.true_airspeed_m_s * sqrtf(_wing_l.get_rho() / RHO);
    airspeed.air_temperature_celsius = NAN;
    airspeed.confidence              = 0.7f;
    airspeed.timestamp               = hrt_absolute_time();
    _airspeed_pub.publish(airspeed);
}

void Sih::send_dist_snsr(const hrt_abstime &time_now_us) {
    device::DeviceId device_id;
    device_id.devid_s.bus_type  = device::DeviceBusType::DeviceBusType_SIMULATION;
    device_id.devid_s.bus_index = 0;
    device_id.devid_s.address   = 0;
    device_id.devid_s.devtype   = DRV_DIST_DEVTYPE_SIM;

    distance_sensor_s distance_sensor{};
    // distance_sensor.timestamp_sample = time_now_us;
    distance_sensor.device_id      = device_id.devid;
    distance_sensor.type           = distance_sensor_s::MAV_DISTANCE_SENSOR_LASER;
    distance_sensor.orientation    = distance_sensor_s::ROTATION_DOWNWARD_FACING;
    distance_sensor.min_distance   = _distance_snsr_min;
    distance_sensor.max_distance   = _distance_snsr_max;
    distance_sensor.signal_quality = -1;

    if (_distance_snsr_override >= 0.f) {
        distance_sensor.current_distance = _distance_snsr_override;

    } else {
        distance_sensor.current_distance = -_p_I(2) / _C_IB(2, 2);

        if (distance_sensor.current_distance > _distance_snsr_max) {
            // this is based on lightware lw20 behaviour
            distance_sensor.current_distance = UINT16_MAX / 100.f;
        }
    }

    distance_sensor.timestamp = hrt_absolute_time();
    _distance_snsr_pub.publish(distance_sensor);
}

void Sih::publish_ground_truth(const hrt_abstime &time_now_us) {
    {
        // publish angular velocity groundtruth
        vehicle_angular_velocity_s angular_velocity{};
        angular_velocity.timestamp_sample = time_now_us;
        angular_velocity.xyz[0]           = _w_B(0); // rollspeed;
        angular_velocity.xyz[1]           = _w_B(1); // pitchspeed;
        angular_velocity.xyz[2]           = _w_B(2); // yawspeed;
        angular_velocity.timestamp        = hrt_absolute_time();
        _angular_velocity_ground_truth_pub.publish(angular_velocity);
    }

    {
        // publish attitude groundtruth
        vehicle_attitude_s attitude{};
        attitude.timestamp_sample = time_now_us;
        _q.copyTo(attitude.q);
        attitude.timestamp = hrt_absolute_time();
        _attitude_ground_truth_pub.publish(attitude);
    }

    {
        // publish local position groundtruth
        vehicle_local_position_s local_position{};
        local_position.timestamp_sample = time_now_us;

        local_position.xy_valid   = true;
        local_position.z_valid    = true;
        local_position.v_xy_valid = true;
        local_position.v_z_valid  = true;

        local_position.x = _p_I(0);
        local_position.y = _p_I(1);
        local_position.z = _p_I(2);

        local_position.vx      = _v_I(0);
        local_position.vy      = _v_I(1);
        local_position.vz      = _v_I(2);
        local_position.z_deriv = _v_I(2);

        local_position.ax = _v_I_dot(0);
        local_position.ay = _v_I_dot(1);
        local_position.az = _v_I_dot(2);

        local_position.xy_global     = true;
        local_position.z_global      = true;
        local_position.ref_timestamp = _last_run;
        local_position.ref_lat       = _LAT0;
        local_position.ref_lon       = _LON0;
        local_position.ref_alt       = _H0;

        local_position.heading                  = Eulerf(_q).psi();
        local_position.heading_good_for_control = true;

        local_position.timestamp = hrt_absolute_time();
        _local_position_ground_truth_pub.publish(local_position);
    }

    {
        // publish global position groundtruth
        vehicle_global_position_s global_position{};
        global_position.timestamp_sample = time_now_us;
        global_position.lat              = _LAT0 + degrees((double)_p_I(0) / CONSTANTS_RADIUS_OF_EARTH);
        global_position.lon              = _LON0 + degrees((double)_p_I(1) / CONSTANTS_RADIUS_OF_EARTH) / _COS_LAT0;
        global_position.alt              = _H0 - _p_I(2);
        global_position.alt_ellipsoid    = global_position.alt;
        global_position.terrain_alt      = -_p_I(2);
        global_position.timestamp        = hrt_absolute_time();
        _global_position_ground_truth_pub.publish(global_position);
    }
}

float Sih::generate_wgn() // generate white Gaussian noise sample with std=1
{
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

Vector3f Sih::noiseGauss3f(float stdx, float stdy, float stdz) {
    return Vector3f(generate_wgn() * stdx, generate_wgn() * stdy, generate_wgn() * stdz);
}

int Sih::print_status() {
#if defined(ENABLE_LOCKSTEP_SCHEDULER)
    rt_kprintf("Running in lockstep mode\n");
    rt_kprintf("Achieved speedup: %.2fX\n", (double)_achieved_speedup);
#endif

    if (_vehicle == VehicleType::MC) {
        rt_kprintf("Running MultiCopter\n");

    } else if (_vehicle == VehicleType::FW) {
        rt_kprintf("Running Fixed-Wing\n");

    } else if (_vehicle == VehicleType::TS) {
        rt_kprintf("Running TailSitter\n");
        rt_kprintf("aoa [deg]: %d\n", (int)(degrees(_ts[4].get_aoa())));
        rt_kprintf("v segment (m/s)\n");
        _ts[4].get_vS().print();
    }

    rt_kprintf("vehicle landed: %d\n", _grounded);
    rt_kprintf("inertial position NED (m)\n");
    _p_I.print();
    rt_kprintf("inertial velocity NED (m/s)\n");
    _v_I.print();
    rt_kprintf("attitude roll-pitch-yaw (deg)\n");
    (Eulerf(_q) * 180.0f / M_PI_F).print();
    rt_kprintf("angular acceleration roll-pitch-yaw (deg/s)\n");
    (_w_B * 180.0f / M_PI_F).print();
    rt_kprintf("actuator signals\n");
    Vector<float, 8> u = Vector<float, 8>(_u);
    u.transpose().print();
    rt_kprintf("Aerodynamic forces NED inertial (N)\n");
    _Fa_I.print();
    rt_kprintf("Aerodynamic moments body frame (Nm)\n");
    _Ma_B.print();
    rt_kprintf("Thruster moments in body frame (Nm)\n");
    _Mt_B.print();
    return 0;
}

// int Sih::task_spawn(int argc, char *argv[]) {
//     _task_id = px4_task_spawn_cmd("sih",
//                                   SCHED_DEFAULT,
//                                   SCHED_PRIORITY_MAX,
//                                   1250,
//                                   (px4_main_t)&run_trampoline,
//                                   (char *const *)argv);

//     if (_task_id < 0) {
//         _task_id = -1;
//         return -errno;
//     }

//     return 0;
// }

Sih *Sih::instantiate(int argc, char *argv[]) {
    Sih *instance = new Sih();

    if (instance == nullptr) {
        PX4_ERR("alloc failed");
    }

    return instance;
}

int Sih::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int Sih::print_usage(const char *reason) {
    if (reason) {
        PX4_WARN("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
This module provides a simulator for quadrotors and fixed-wings running fully
inside the hardware autopilot.

This simulator subscribes to "actuator_outputs" which are the actuator pwm
signals given by the control allocation module.

This simulator publishes the sensors signals corrupted with realistic noise
in order to incorporate the state estimator in the loop.

### Implementation
The simulator implements the equations of motion using matrix algebra.
Quaternion representation is used for the attitude.
Forward Euler is used for integration.
Most of the variables are declared global in the .hpp file to avoid stack overflow.


)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("simulator_sih", "simulation");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

extern "C" __EXPORT int simulator_sih_main(int argc, char *argv[]) {
    return Sih::main(argc, argv);
}

MSH_CMD_EXPORT_ALIAS(simulator_sih_main, sih, simulation in hardware);

int simulator_sih_start() {
    int32_t hitl = param_get_int32((param_t)params_id::SYS_HITL);

    if (hitl == 2) {
        const char *argv[] = {"sih", "start"};
        int         argc   = sizeof(argv) / sizeof(argv[0]);
        return Sih::main(argc, (char **)argv);
    }

    return 0;
}

INIT_APP_EXPORT(simulator_sih_start);
