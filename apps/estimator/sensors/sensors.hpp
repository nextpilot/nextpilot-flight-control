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
 * @file sensors.hpp
 *
 * @author Lorenz Meier <lorenz@px4.io>
 * @author Julian Oes <julian@oes.ch>
 * @author Thomas Gubler <thomas@px4.io>
 * @author Anton Babushkin <anton@px4.io>
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#include <sensor_calibration/Utilities.hpp>
#include <getopt/getopt.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include "voted_sensors_update.h"
#include "vehicle_imu/VehicleIMU.hpp"

#if defined(SENSORS_VEHICLE_ACCELERATION)
#   include "vehicle_acceleration/VehicleAcceleration.hpp"
#endif // SENSORS_VEHICLE_ACCELERATION

#if defined(SENSORS_VEHICLE_AIRSPEED)
#   include <device/device_id.h>
#   include <drivers/drv_adc.h>
#   include <airspeed/airspeed.h>
#   include <uORB/topics/airspeed.h>
#   include <uORB/topics/differential_pressure.h>
#   include <uORB/topics/vehicle_air_data.h>
#endif // SENSORS_VEHICLE_AIRSPEED

#if defined(SENSORS_VEHICLE_AIR_DATA)
#   include <uORB/topics/sensor_baro.h>
#   include "vehicle_air_data/VehicleAirData.hpp"
#endif // SENSORS_VEHICLE_AIR_DATA

#if defined(SENSORS_VEHICLE_ANGULAR_VELOCITY)
#   include "vehicle_angular_velocity/VehicleAngularVelocity.hpp"
#endif // SENSORS_VEHICLE_ANGULAR_VELOCITY

#if defined(SENSORS_VEHICLE_GPS_POSITION)
#   include "vehicle_gps_position/VehicleGPSPosition.hpp"
#endif // SENSORS_VEHICLE_GPS_POSITION

#if defined(SENSORS_VEHICLE_MAGNETOMETER)
#   include "vehicle_magnetometer/VehicleMagnetometer.hpp"
#   include <sensor_calibration/Magnetometer.hpp>
#   include <uORB/topics/sensor_mag.h>
#endif // SENSORS_VEHICLE_MAGNETOMETER

#if defined(SENSORS_VEHICLE_OPTICAL_FLOW)
#   include "vehicle_optical_flow/VehicleOpticalFlow.hpp"
#endif // SENSORS_VEHICLE_OPTICAL_FLOW

using namespace sensors;
using namespace time_literals;
/**
 * HACK - true temperature is much less than indicated temperature in baro,
 * subtract 5 degrees in an attempt to account for the electrical upheating of the PCB
 */
#define PCB_TEMP_ESTIMATE_DEG 5.0f

class Sensors : public ModuleCommand<Sensors>, public ModuleParams, public WorkItemScheduled {
public:
    explicit Sensors(bool hil_enabled);
    ~Sensors() override;

    /** @see ModuleCommand */
    static Sensors *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::run() */
    void Run() override;

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    int init() override;

private:
    int parameters_update();

    void InitializeVehicleAirData();

    void InitializeVehicleGPSPosition();

    void InitializeVehicleIMU();

    void InitializeVehicleMagnetometer();

    void InitializeVehicleOpticalFlow();

    const bool _hil_enabled;   /**< if true, HIL is active */

    perf_counter_t _loop_perf; /**< loop performance counter */

    VehicleIMU *_vehicle_imu_list[MAX_SENSOR_COUNT]{};

    VotedSensorsUpdate _voted_sensors_update;

    sensor_combined_s _sensor_combined{};

    hrt_abstime _last_config_update{0};
    hrt_abstime _sensor_combined_prev_timestamp{0};

    uint8_t _n_accel{0};
    uint8_t _n_gyro{0};

    bool _armed{false}; /**< arming status of the vehicle */

    uORB::SubscriptionCallbackWorkItem _vehicle_imu_sub[MAX_SENSOR_COUNT]{
        {this, ORB_ID(vehicle_imu), 0},
        {this, ORB_ID(vehicle_imu), 1},
        {this, ORB_ID(vehicle_imu), 2},
        {this, ORB_ID(vehicle_imu), 3}};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};
    uORB::Subscription         _vcontrol_mode_sub{ORB_ID(vehicle_control_mode)};

    uORB::Publication<sensor_combined_s> _sensor_pub{ORB_ID(sensor_combined)};

#if defined(SENSORS_VEHICLE_AIRSPEED)
    /**
     * Poll the differential pressure sensor for updated data.
     *
     * @param raw	Combined sensor data structure into which
     *		data should be returned.
     */
    void diff_pres_poll();

    /**
     * Poll the ADC and update readings to suit.
     *
     * @param raw	Combined sensor data structure into which
     *		data should be returned.
     */
    void adc_poll();

    uORB::Subscription _diff_pres_sub{ORB_ID(differential_pressure)};
    uORB::Subscription _vehicle_air_data_sub{ORB_ID(vehicle_air_data)};

    uORB::Publication<airspeed_s> _airspeed_pub{ORB_ID(airspeed)};

    DataValidator _airspeed_validator; /**< data validator to monitor airspeed */

    float _diff_pres_pressure_sum{0.f};
    float _diff_pres_temperature_sum{0.f};
    float _baro_pressure_sum{0.f};

    int _diff_pres_count{0};

    uint64_t _airspeed_last_publish{0};
    uint64_t _diff_pres_timestamp_sum{0};

#   ifdef ADC_AIRSPEED_VOLTAGE_CHANNEL
    uORB::Subscription                              _adc_report_sub{ORB_ID(adc_report)};
    uORB::PublicationMulti<differential_pressure_s> _diff_pres_pub{ORB_ID(differential_pressure)};
#   endif // ADC_AIRSPEED_VOLTAGE_CHANNEL

    struct Parameters {
        float diff_pres_offset_pa;
#   ifdef ADC_AIRSPEED_VOLTAGE_CHANNEL
        float diff_pres_analog_scale;
#   endif /* ADC_AIRSPEED_VOLTAGE_CHANNEL */

        int32_t air_cmodel;
        float   air_tube_length;
        float   air_tube_diameter_mm;
    } _parameters{}; /**< local copies of interesting parameters */

    struct ParameterHandles {
        param_t diff_pres_offset_pa;
#   ifdef ADC_AIRSPEED_VOLTAGE_CHANNEL
        param_t diff_pres_analog_scale;
#   endif /* ADC_AIRSPEED_VOLTAGE_CHANNEL */

        param_t air_cmodel;
        param_t air_tube_length;
        param_t air_tube_diameter_mm;
    } _parameter_handles{}; /**< handles for interesting parameters */
#endif                      // SENSORS_VEHICLE_AIRSPEED

#if defined(SENSORS_VEHICLE_ACCELERATION)
    VehicleAcceleration _vehicle_acceleration;
#endif // SENSORS_VEHICLE_ACCELERATION

#if defined(SENSORS_VEHICLE_AIR_DATA)
    VehicleAirData *_vehicle_air_data{nullptr};
    uint8_t         _n_baro{0};
#endif // SENSORS_VEHICLE_AIR_DATA

#if defined(SENSORS_VEHICLE_ANGULAR_VELOCITY)
    VehicleAngularVelocity _vehicle_angular_velocity;
#endif // SENSORS_VEHICLE_ANGULAR_VELOCITY

#if defined(SENSORS_VEHICLE_MAGNETOMETER)
    VehicleMagnetometer *_vehicle_magnetometer{nullptr};
    uint8_t              _n_mag{0};
#endif // SENSORS_VEHICLE_MAGNETOMETER

#if defined(SENSORS_VEHICLE_GPS_POSITION)
    VehicleGPSPosition *_vehicle_gps_position{nullptr};
    uint8_t             _n_gps{0};
#endif // SENSORS_VEHICLE_GPS_POSITION

#if defined(SENSORS_VEHICLE_OPTICAL_FLOW)
    VehicleOpticalFlow *_vehicle_optical_flow{nullptr};
    uint8_t             _n_optical_flow{0};
#endif // SENSORS_VEHICLE_OPTICAL_FLOW

    DEFINE_PARAMETERS(
#if defined(SENSORS_VEHICLE_AIR_DATA)
        (ParamBool<params_id::SYS_HAS_BARO>)_param_sys_has_baro,
#endif // SENSORS_VEHICLE_AIR_DATA
#if defined(SENSORS_VEHICLE_GPS_POSITION)
        (ParamBool<params_id::SYS_HAS_GPS>)_param_sys_has_gps,
#endif // SENSORS_VEHICLE_GPS_POSITION
#if defined(SENSORS_VEHICLE_MAGNETOMETER)
        (ParamInt<params_id::SYS_HAS_MAG>)_param_sys_has_mag,
#endif // SENSORS_VEHICLE_MAGNETOMETER
        (ParamBool<params_id::SENS_IMU_MODE>)_param_sens_imu_mode)
};
