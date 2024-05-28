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
#include <uORB/topics/airspeed_validated.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

class AirspeedChecks : public HealthAndArmingCheckBase {
public:
    AirspeedChecks();
    ~AirspeedChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::Subscription _airspeed_validated_sub{ORB_ID(airspeed_validated)};

    const param_t _param_fw_arsp_mode_handle;
    const param_t _param_fw_airspd_max_handle;

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<params_id::CBRK_AIRSPD_CHK>)_param_cbrk_airspd_chk,
                                    (ParamBool<params_id::COM_ARM_ARSP_EN>)_param_com_arm_arsp_en)
};
