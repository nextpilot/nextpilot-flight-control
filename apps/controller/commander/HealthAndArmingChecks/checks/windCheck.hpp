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
#include <uORB/topics/wind.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

class WindChecks : public HealthAndArmingCheckBase {
public:
    WindChecks()  = default;
    ~WindChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::Subscription _wind_sub{ORB_ID(wind)};
    hrt_abstime        _last_wind_warning{0};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamFloat<params_id::COM_WIND_MAX>)_param_com_wind_max,
                                    (ParamFloat<params_id::COM_WIND_WARN>)_param_com_wind_warn

    )
};
