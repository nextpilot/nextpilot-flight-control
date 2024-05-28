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
#include <uORB/topics/actuator_armed.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

class SystemChecks : public HealthAndArmingCheckBase {
public:
    SystemChecks()  = default;
    ~SystemChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::Subscription _actuator_armed_sub{ORB_ID(actuator_armed)};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<params_id::CBRK_VTOLARMING>)_param_cbrk_vtolarming,
                                    (ParamInt<params_id::CBRK_USB_CHK>)_param_cbrk_usb_chk,
                                    (ParamBool<params_id::COM_ARM_WO_GPS>)_param_com_arm_wo_gps,
                                    (ParamInt<params_id::COM_ARM_AUTH_REQ>)_param_com_arm_auth_req,
                                    (ParamInt<params_id::GF_ACTION>)_param_gf_action)
};
