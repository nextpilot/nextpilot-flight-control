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
#include <uORB/topics/system_power.h>

using namespace nextpilot;
using namespace nextpilot::param;

class PowerChecks : public HealthAndArmingCheckBase {
public:
    PowerChecks()  = default;
    ~PowerChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::Subscription _system_power_sub{ORB_ID(system_power)};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<params_id::CBRK_SUPPLY_CHK>)_param_cbrk_supply_chk,
                                    (ParamInt<params_id::COM_POWER_COUNT>)_param_com_power_count)
};
