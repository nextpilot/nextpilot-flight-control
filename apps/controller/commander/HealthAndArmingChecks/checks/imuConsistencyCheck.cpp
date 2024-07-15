/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "imu_check"
#define LOG_LVL LOG_LVL_INFO

#include "imuConsistencyCheck.hpp"

void ImuConsistencyChecks::checkAndReport(const Context &context, Report &reporter) {
    sensors_status_imu_s imu;

    if (!_sensors_status_imu_sub.copy(&imu) || context.isArmed()) {
        return;
    }

    // Use the difference between IMU's to detect a bad calibration.
    // If a single IMU is fitted, the value being checked will be zero so this check will always pass.
    for (unsigned i = 0; i < (sizeof(imu.accel_inconsistency_m_s_s) / sizeof(imu.accel_inconsistency_m_s_s[0])); i++) {
        if (imu.accel_device_ids[i] != 0) {
            const float accel_inconsistency_m_s_s = imu.accel_inconsistency_m_s_s[i];

            NavModes required_groups = NavModes::None;

            if (accel_inconsistency_m_s_s > _param_com_arm_imu_acc.get()) {
                required_groups = NavModes::All;
            }

            if (accel_inconsistency_m_s_s > _param_com_arm_imu_acc.get() * 0.8f) {
                /* EVENT
                 * @description
                 * Check the calibration.
                 *
                 * Inconsistency value: {2}.
                 * Configured Threshold: {3}.
                 *
                 * <profile name="dev">
                 * This check can be configured via <param>COM_ARM_IMU_ACC</param> parameter.
                 * </profile>
                 */
                reporter.armingCheckFailure<uint8_t, float, float>(required_groups, health_component_t::accel,
                                                                   events::ID("check_imu_accel_inconsistent"),
                                                                   events::Log::Warning, "Accel {1} inconsistent", i, accel_inconsistency_m_s_s, _param_com_arm_imu_acc.get());

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Accel %u inconsistent - check cal", i);
                }

                break;
            }
        }
    }

    // Fail if gyro difference greater than 5 deg/sec and notify if greater than 2.5 deg/sec
    for (unsigned i = 0; i < (sizeof(imu.gyro_inconsistency_rad_s) / sizeof(imu.gyro_inconsistency_rad_s[0])); i++) {
        if (imu.gyro_device_ids[i] != 0) {
            const float gyro_inconsistency_rad_s = imu.gyro_inconsistency_rad_s[i];

            NavModes required_groups = NavModes::None;

            if (gyro_inconsistency_rad_s > _param_com_arm_imu_gyr.get()) {
                required_groups = NavModes::All;
            }

            if (gyro_inconsistency_rad_s > _param_com_arm_imu_gyr.get() * 0.5f) {
                /* EVENT
                 * @description
                 * Check the calibration.
                 *
                 * Inconsistency value: {2}.
                 * Configured Threshold: {3}.
                 *
                 * <profile name="dev">
                 * This check can be configured via <param>COM_ARM_IMU_GYR</param> parameter.
                 * </profile>
                 */
                reporter.armingCheckFailure<uint8_t, float, float>(required_groups, health_component_t::gyro,
                                                                   events::ID("check_imu_gyro_inconsistent"),
                                                                   events::Log::Warning, "Gyro {1} inconsistent", i, gyro_inconsistency_rad_s, _param_com_arm_imu_gyr.get());

                if (reporter.mavlink_log_pub()) {
                    mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: Gyro %u inconsistent - check cal", i);
                }

                break;
            }
        }
    }
}
