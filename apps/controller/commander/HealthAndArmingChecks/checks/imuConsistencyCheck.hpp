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
#include <uORB/topics/sensors_status_imu.h>

class ImuConsistencyChecks : public HealthAndArmingCheckBase {
public:
    ImuConsistencyChecks()  = default;
    ~ImuConsistencyChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::Subscription _sensors_status_imu_sub{ORB_ID(sensors_status_imu)};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamFloat<params_id::COM_ARM_IMU_ACC>)_param_com_arm_imu_acc,
                                    (ParamFloat<params_id::COM_ARM_IMU_GYR>)_param_com_arm_imu_gyr)
};
