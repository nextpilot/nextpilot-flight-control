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
#include <uORB/topics/cpuload.h>

class CpuResourceChecks : public HealthAndArmingCheckBase {
public:
    CpuResourceChecks()  = default;
    ~CpuResourceChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    uORB::Subscription _cpuload_sub{ORB_ID(cpuload)};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamFloat<params_id::COM_CPU_MAX>)_param_com_cpu_max)
};
