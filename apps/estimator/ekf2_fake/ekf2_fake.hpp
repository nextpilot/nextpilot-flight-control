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

// #define USING_COMMANDER_FAKE

class EKF2FAKE {
public:
    EKF2FAKE();
    ~EKF2FAKE();

    int  init();
    void Run();
    void publish_imu_data(const hrt_abstime now_us);
    void publish_gps_data(const hrt_abstime now_us);
    void publish_ekf_data(const hrt_abstime now_us);
    void publish_est_data(const hrt_abstime now_us);
#ifdef USING_COMMANDER_FAKE
    void publish_cmd_data(const hrt_abstime now_us);
#endif // USING_COMMANDER_FAKE

private:
    sensor_accel_s             _sacc{};
    uORB::Subscription         _sacc_sub{ORB_ID(sensor_accel)};
    sensor_gps_s               _sgps{};
    uORB::Subscription         _sgps_sub{ORB_ID(sensor_gps)};
    vehicle_angular_velocity_s _vangvel_gt{};
    uORB::Subscription         _vangvel_gt_sub{ORB_ID(vehicle_angular_velocity_groundtruth)};
    vehicle_attitude_s         _vatt_gt{};
    uORB::Subscription         _vatt_gt_sub{ORB_ID(vehicle_attitude_groundtruth)};
    vehicle_local_position_s   _vlpos_gt{};
    uORB::Subscription         _vlpos_gt_sub{ORB_ID(vehicle_local_position_groundtruth)};
    vehicle_global_position_s  _vgpos_gt{};
    uORB::Subscription         _vgpos_gt_sub{ORB_ID(vehicle_global_position_groundtruth)};
    /***********************************************************************************************/
    vehicle_angular_velocity_s                    _vangvel{};
    uORB::Publication<vehicle_angular_velocity_s> _vangvel_pub{ORB_ID(vehicle_angular_velocity)};
    vehicle_acceleration_s                        _vacc{};
    uORB::Publication<vehicle_acceleration_s>     _vacc_pub{ORB_ID(vehicle_acceleration)};
    sensor_gps_s                                  _vgpspos{};
    uORB::Publication<sensor_gps_s>               _vgpspos_pub{ORB_ID(vehicle_gps_position)};
    vehicle_attitude_s                            _vatt{};
    uORB::Publication<vehicle_attitude_s>         _vatt_pub{ORB_ID(vehicle_attitude)};
    vehicle_local_position_s                      _vlpos{};
    uORB::Publication<vehicle_local_position_s>   _vlpos_pub{ORB_ID(vehicle_local_position)};
    vehicle_global_position_s                     _vgpos{};
    uORB::Publication<vehicle_global_position_s>  _vgpos_pub{ORB_ID(vehicle_global_position)};
    estimator_status_s                            _est_status{};
    uORB::Publication<estimator_status_s>         _est_status_pub{ORB_ID(estimator_status)};
    /***********************************************************************************************/
#ifdef USING_COMMANDER_FAKE
    actuator_armed_s                             _armed{};
    uORB::Publication<actuator_armed_s>          _armed_pub{ORB_ID(actuator_armed)};
    actuator_test_s                              _actuator_test{};
    uORB::Publication<actuator_test_s>           _actuator_test_pub{ORB_ID(actuator_test)};
    failure_detector_status_s                    _fd_status{};
    uORB::Publication<failure_detector_status_s> _fd_status_pub{ORB_ID(failure_detector_status)};
    vehicle_command_ack_s                        _vcmd_ack{};
    uORB::Publication<vehicle_command_ack_s>     _vcmd_ack_pub{ORB_ID(vehicle_command_ack)};
    vehicle_control_mode_s                       _vctr_mode{};
    uORB::Publication<vehicle_control_mode_s>    _vctr_mode_pub{ORB_ID(vehicle_control_mode)};
    vehicle_status_s                             _vstatus{};
    uORB::Publication<vehicle_status_s>          _vstatus_pub{ORB_ID(vehicle_status)};
#endif // USING_COMMANDER_FAKE
};
