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

#include "GZMixingInterfaceESC.hpp"
#include "GZMixingInterfaceServo.hpp"

#include <atomic/atomic.hpp>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>

#include <workq/WorkItemScheduled.hpp>
#include <geo/geo.h>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/differential_pressure.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/vehicle_angular_velocity.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/vehicle_odometry.h>

#include <gz/math.hh>
#include <gz/msgs.hh>
#include <gz/transport.hh>

#include <gz/msgs/imu.pb.h>
#include <gz/msgs/fluid_pressure.pb.h>
#include <gz/msgs/odometry_with_covariance.pb.h>

using namespace time_literals;

class GZBridge : public ModuleCommand<GZBridge>, public ModuleParams, public WorkItemScheduled {
public:
    GZBridge(const char *world, const char *name, const char *model, const char *pose_str);
    ~GZBridge() override;

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand */
    static int *instantiate(int argc, char *argv[]);

    int init();

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    uint64_t world_time_us() const {
        return _world_time_us.load();
    }

private:
    void Run() override;

    bool updateClock(const uint64_t tv_sec, const uint64_t tv_nsec);

    void clockCallback(const gz::msgs::Clock &clock);

    // void airspeedCallback(const gz::msgs::AirSpeedSensor &air_pressure);
    void barometerCallback(const gz::msgs::FluidPressure &air_pressure);
    void imuCallback(const gz::msgs::IMU &imu);
    void poseInfoCallback(const gz::msgs::Pose_V &pose);
    void odometryCallback(const gz::msgs::OdometryWithCovariance &odometry);

    /**
     *
     * Convert a quaterion from FLU_to_ENU frames (ROS convention)
     * to FRD_to_NED frames (PX4 convention)
     *
     * @param q_FRD_to_NED output quaterion in PX4 conventions
     * @param q_FLU_to_ENU input quaterion in ROS conventions
     */
    static void rotateQuaternion(gz::math::Quaterniond &q_FRD_to_NED, const gz::math::Quaterniond q_FLU_to_ENU);

    // Subscriptions
    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    // uORB::Publication<differential_pressure_s>    _differential_pressure_pub{ORB_ID(differential_pressure)};
    uORB::Publication<vehicle_angular_velocity_s> _angular_velocity_ground_truth_pub{ORB_ID(vehicle_angular_velocity_groundtruth)};
    uORB::Publication<vehicle_attitude_s>         _attitude_ground_truth_pub{ORB_ID(vehicle_attitude_groundtruth)};
    uORB::Publication<vehicle_global_position_s>  _gpos_ground_truth_pub{ORB_ID(vehicle_global_position_groundtruth)};
    uORB::Publication<vehicle_local_position_s>   _lpos_ground_truth_pub{ORB_ID(vehicle_local_position_groundtruth)};
    uORB::PublicationMulti<sensor_baro_s>         _sensor_baro_pub{ORB_ID(sensor_baro)};

    uORB::PublicationMulti<sensor_accel_s>     _sensor_accel_pub{ORB_ID(sensor_accel)};
    uORB::PublicationMulti<sensor_gyro_s>      _sensor_gyro_pub{ORB_ID(sensor_gyro)};
    uORB::PublicationMulti<vehicle_odometry_s> _visual_odometry_pub{ORB_ID(vehicle_visual_odometry)};

    GZMixingInterfaceESC   _mixing_interface_esc{_node, _node_mutex};
    GZMixingInterfaceServo _mixing_interface_servo{_node, _node_mutex};

    atomic<uint64_t> _world_time_us{0};

    pthread_mutex_t _node_mutex;

    MapProjection _pos_ref{};

    matrix::Vector3d _position_prev{};
    matrix::Vector3d _velocity_prev{};
    matrix::Vector3f _euler_prev{};
    hrt_abstime      _timestamp_prev{};

    const std::string _world_name;
    const std::string _model_name;
    const std::string _model_sim;
    const std::string _model_pose;

    float _temperature{288.15}; // 15 degrees

    gz::transport::Node _node;

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::SIM_GZ_HOME_LAT>)_param_sim_home_lat,
        (ParamFloat<params_id::SIM_GZ_HOME_LON>)_param_sim_home_lon,
        (ParamFloat<params_id::SIM_GZ_HOME_ALT>)_param_sim_home_alt)
};
