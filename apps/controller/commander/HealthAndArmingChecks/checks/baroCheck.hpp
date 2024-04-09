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
#include <uORB/topics/sensor_baro.h>
#include <uORB/topics/estimator_status.h>
#include <lib/sensor_calibration/Barometer.hpp>

class BaroChecks : public HealthAndArmingCheckBase {
public:
    BaroChecks()  = default;
    ~BaroChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    bool isBaroRequired(int instance);

    uORB::SubscriptionMultiArray<sensor_baro_s, calibration::Barometer::MAX_SENSOR_COUNT> _sensor_baro_sub{ORB_ID::sensor_baro};
    uORB::SubscriptionMultiArray<estimator_status_s>                                      _estimator_status_sub{ORB_ID::estimator_status};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamBool<params_id::SYS_HAS_BARO>)_param_sys_has_baro)
};
