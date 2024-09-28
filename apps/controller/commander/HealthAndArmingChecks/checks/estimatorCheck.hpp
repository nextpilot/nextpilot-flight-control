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

#include "../Common.hpp"
#include <uORB/Subscription.hpp>
#include <uORB/topics/estimator_selector_status.h>
#include <uORB/topics/estimator_sensor_bias.h>
#include <uORB/topics/estimator_status.h>
#include <uORB/topics/estimator_status_flags.h>
#include <uORB/topics/vehicle_angular_velocity.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/sensor_gps.h>
#include <hysteresis/hysteresis.h>

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::param;

class EstimatorChecks : public HealthAndArmingCheckBase {
public:
    EstimatorChecks();
    ~EstimatorChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    void checkEstimatorStatus(const Context &context, Report &reporter, const estimator_status_s &estimator_status,
                              NavModes required_groups);
    void checkSensorBias(const Context &context, Report &reporter, NavModes required_groups);
    void checkEstimatorStatusFlags(const Context &context, Report &reporter, const estimator_status_s &estimator_status,
                                   const vehicle_local_position_s &lpos);

    void checkGps(const Context &context, Report &reporter, const sensor_gps_s &vehicle_gps_position) const;
    void lowPositionAccuracy(const Context &context, Report &reporter, const vehicle_local_position_s &lpos) const;
    void setModeRequirementFlags(const Context &context, bool pre_flt_fail_innov_heading, bool pre_flt_fail_innov_vel_horiz,
                                 const vehicle_local_position_s &lpos, const sensor_gps_s &vehicle_gps_position,
                                 failsafe_flags_s &failsafe_flags);

    bool checkPosVelValidity(const hrt_abstime &now, const bool data_valid, const float data_accuracy,
                             const float        required_accuracy,
                             const hrt_abstime &data_timestamp_us, hrt_abstime &last_fail_time_us,
                             const bool was_valid) const;

    uORB::Subscription _estimator_selector_status_sub{ORB_ID(estimator_selector_status)};
    uORB::Subscription _estimator_status_sub{ORB_ID(estimator_status)};
    uORB::Subscription _estimator_status_flags_sub{ORB_ID(estimator_status_flags)};
    uORB::Subscription _estimator_sensor_bias_sub{ORB_ID(estimator_sensor_bias)};

    uORB::Subscription _vehicle_global_position_sub{ORB_ID(vehicle_global_position)};
    uORB::Subscription _vehicle_local_position_sub{ORB_ID(vehicle_local_position)};
    uORB::Subscription _vehicle_angular_velocity_sub{ORB_ID(vehicle_angular_velocity)};
    uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _vehicle_gps_position_sub{ORB_ID(vehicle_gps_position)};

    hrt_abstime _last_gpos_fail_time_us{0};         ///< Last time that the global position validity recovery check failed (usec)
    hrt_abstime _last_lpos_fail_time_us{0};         ///< Last time that the local position validity recovery check failed (usec)
    hrt_abstime _last_lpos_relaxed_fail_time_us{0}; ///< Last time that the relaxed local position validity recovery check failed (usec)
    hrt_abstime _last_lvel_fail_time_us{0};         ///< Last time that the local velocity validity recovery check failed (usec)

    // variables used to check for navigation failure after takeoff
    hrt_abstime _time_last_innov_pass{0}; ///< last time velocity and position innovations passed
    hrt_abstime _time_last_innov_fail{0}; ///< last time velocity and position innovations failed
    bool        _nav_test_passed{false};  ///< true if the post takeoff navigation test has passed
    bool        _nav_test_failed{false};  ///< true if the post takeoff navigation test has failed

    bool _position_reliant_on_optical_flow{false};

    bool _gps_was_fused{false};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<params_id::SYS_MC_EST_GROUP>)_param_sys_mc_est_group,
                                    (ParamInt<params_id::SENS_IMU_MODE>)_param_sens_imu_mode,
                                    (ParamInt<params_id::COM_ARM_MAG_STR>)_param_com_arm_mag_str,
                                    (ParamFloat<params_id::COM_ARM_EKF_HGT>)_param_com_arm_ekf_hgt,
                                    (ParamFloat<params_id::COM_ARM_EKF_VEL>)_param_com_arm_ekf_vel,
                                    (ParamFloat<params_id::COM_ARM_EKF_POS>)_param_com_arm_ekf_pos,
                                    (ParamFloat<params_id::COM_ARM_EKF_YAW>)_param_com_arm_ekf_yaw,
                                    (ParamBool<params_id::COM_ARM_WO_GPS>)_param_com_arm_wo_gps,
                                    (ParamBool<params_id::SYS_HAS_GPS>)_param_sys_has_gps,
                                    (ParamFloat<params_id::COM_POS_FS_EPH>)_param_com_pos_fs_eph,
                                    (ParamFloat<params_id::COM_VEL_FS_EVH>)_param_com_vel_fs_evh,
                                    (ParamInt<params_id::COM_POS_FS_DELAY>)_param_com_pos_fs_delay,
                                    (ParamFloat<params_id::COM_POS_LOW_EPH>)_param_com_low_eph)
};
