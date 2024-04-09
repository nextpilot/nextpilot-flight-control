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
#include <uORB/topics/sensor_preflight_mag.h>
#include <uORB/topics/sensor_mag.h>
#include <uORB/topics/estimator_status.h>
#include <lib/sensor_calibration/Magnetometer.hpp>

class MagnetometerChecks : public HealthAndArmingCheckBase {
public:
    MagnetometerChecks()  = default;
    ~MagnetometerChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    bool isMagRequired(int instance, bool &mag_fault);
    void consistencyCheck(const Context &context, Report &reporter);

    uORB::SubscriptionMultiArray<sensor_mag_s, calibration::Magnetometer::MAX_SENSOR_COUNT> _sensor_mag_sub{ORB_ID::sensor_mag};
    uORB::SubscriptionMultiArray<estimator_status_s>                                        _estimator_status_sub{ORB_ID::estimator_status};

    uORB::Subscription _sensor_preflight_mag_sub{ORB_ID(sensor_preflight_mag)};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<px4::params::SYS_HAS_MAG>)_param_sys_has_mag,
                                    (ParamInt<px4::params::COM_ARM_MAG_ANG>)_param_com_arm_mag_ang)
};
