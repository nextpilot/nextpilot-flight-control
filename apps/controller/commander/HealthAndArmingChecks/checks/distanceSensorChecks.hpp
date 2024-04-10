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

#include <uORB/uORBSubscription.hpp>
#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/topics/distance_sensor.h>

class DistanceSensorChecks : public HealthAndArmingCheckBase {
public:
    DistanceSensorChecks()  = default;
    ~DistanceSensorChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_sub{ORB_ID::distance_sensor};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<params_id::SYS_HAS_NUM_DIST>)_param_sys_has_num_dist)
};
