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
#include <uORB/topics/esc_status.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

class EscChecks : public HealthAndArmingCheckBase {
public:
    EscChecks()  = default;
    ~EscChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    void checkEscStatus(const Context &context, Report &reporter, const esc_status_s &esc_status);

    uORB::Subscription _esc_status_sub{ORB_ID(esc_status)};

    const hrt_abstime _start_time{hrt_absolute_time()};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamBool<params_id::COM_ARM_CHK_ESCS>)_param_escs_checks_required)
};
