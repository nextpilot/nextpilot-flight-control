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

using namespace nextpilot;
using namespace nextpilot::param;

class ParachuteChecks : public HealthAndArmingCheckBase {
public:
    ParachuteChecks()  = default;
    ~ParachuteChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamBool<params_id::COM_PARACHUTE>)_param_com_parachute)
};
