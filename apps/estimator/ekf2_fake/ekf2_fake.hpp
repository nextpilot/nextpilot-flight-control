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

#include "nextpilot.h"

using namespace nextpilot;
using namespace time_literals;
using namespace nextpilot::global_params;

class Ekf2Fake {
public:
    Ekf2Fake();
    ~Ekf2Fake();
    int  init();
    void Run();

private:
    static constexpr int EKF2_FAKE_PERIOD_MS = 5;
    uORB::Subscription   _sensor_gyro_sub{ORB_ID(sensor_gyro)};
    uORB::Subscription   _sensor_acc_sub{ORB_ID(sensor_accel)};
    uORB::Subscription   _sensor_gps_sub{ORB_ID(sensor_gps)};
    uORB::Subscription   _v_ang_vel_groundtruth_sub{ORB_ID(vehicle_angular_velocity_groundtruth)};
    uORB::Subscription   _v_att_groundtruth_sub{ORB_ID(vehicle_attitude_groundtruth)};
    uORB::Subscription   _v_loc_pos_groundtruth_sub{ORB_ID(vehicle_local_position_groundtruth)};
    uORB::Subscription   _v_glob_pos_groundtruth_sub{ORB_ID(vehicle_global_position_groundtruth)};

    sensor_gyro_s              _s_gyro{};
    sensor_accel_s             _s_accel{};
    sensor_gps_s               _s_gps{};
    vehicle_angular_velocity_s _v_ang_vel_groundtruth{};
    vehicle_attitude_s         _v_att_groundtruth{};
    vehicle_local_position_s   _v_loc_pos_groundtruth{};
    vehicle_global_position_s  _v_glob_pos_groundtruth{};
    estimator_status_s         _esttimator_status{};

    uORB::Publication<vehicle_angular_velocity_s> _v_ang_pub{ORB_ID(vehicle_angular_velocity)};     /**< vehicle angular velocity publication */
    uORB::Publication<vehicle_acceleration_s>     _v_acc_pub{ORB_ID(vehicle_acceleration)};         /**< vehicle acceleration publication */
    uORB::Publication<sensor_gps_s>               _v_gps_pos_pub{ORB_ID(vehicle_gps_position)};     /**< vehicle gps position publication */
    uORB::Publication<vehicle_angular_velocity_s> _v_ang_vel_pub{ORB_ID(vehicle_angular_velocity)}; /**< vehicle angular velocity publication */
    uORB::Publication<vehicle_attitude_s>         _v_att_pub{ORB_ID(vehicle_attitude)};             /**< vehicle attitude publication */
    uORB::Publication<vehicle_local_position_s>   _v_loc_pos_pub{ORB_ID(vehicle_local_position)};   /**< vehicle local position publication */
    uORB::Publication<vehicle_global_position_s>  _v_glob_pos_pub{ORB_ID(vehicle_global_position)}; /**< vehicle global position publication */
    uORB::Publication<estimator_status_s>         _estimator_status_pub{ORB_ID(estimator_status)};  /**< estimator status publication */
};
