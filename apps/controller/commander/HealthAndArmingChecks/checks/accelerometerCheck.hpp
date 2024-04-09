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
#include <uORB/topics/sensor_accel.h>
#include <lib/sensor_calibration/Accelerometer.hpp>

class AccelerometerChecks : public HealthAndArmingCheckBase {
public:
    AccelerometerChecks()  = default;
    ~AccelerometerChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    bool isAccelRequired(int instance);

    uORB::SubscriptionMultiArray<sensor_accel_s, calibration::Accelerometer::MAX_SENSOR_COUNT> _sensor_accel_sub{ORB_ID::sensor_accel};
    uORB::SubscriptionMultiArray<estimator_status_s>                                           _estimator_status_sub{ORB_ID::estimator_status};
};
