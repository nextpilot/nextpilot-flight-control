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
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/rtl_time_estimate.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

class BatteryChecks : public HealthAndArmingCheckBase {
public:
    BatteryChecks()  = default;
    ~BatteryChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    void rtlEstimateCheck(const Context &context, Report &reporter, float worst_battery_time_s);

    uORB::SubscriptionMultiArray<battery_status_s, battery_status_s::MAX_INSTANCES> _battery_status_subs{ORB_ID::battery_status};
    uORB::Subscription                                                              _rtl_time_estimate_sub{ORB_ID(rtl_time_estimate)};
    bool                                                                            _last_armed{false};
    bool                                                                            _battery_connected_at_arming[battery_status_s::MAX_INSTANCES]{};
};
