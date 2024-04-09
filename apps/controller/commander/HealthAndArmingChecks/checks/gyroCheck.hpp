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
#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/topics/estimator_status.h>
#include <uORB/topics/sensor_gyro.h>
#include <lib/sensor_calibration/Gyroscope.hpp>

class GyroChecks : public HealthAndArmingCheckBase {
public:
    GyroChecks()  = default;
    ~GyroChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    bool isGyroRequired(int instance);

    uORB::SubscriptionMultiArray<sensor_gyro_s, calibration::Gyroscope::MAX_SENSOR_COUNT> _sensor_gyro_sub{ORB_ID::sensor_gyro};
    uORB::SubscriptionMultiArray<estimator_status_s>                                      _estimator_status_sub{ORB_ID::estimator_status};
};
