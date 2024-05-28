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
#include <uORB/topics/input_rc.h>

using namespace nextpilot;
using namespace nextpilot::global_params;

class RcCalibrationChecks : public HealthAndArmingCheckBase {
public:
    RcCalibrationChecks();
    ~RcCalibrationChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    void updateParams() override;

    struct ParamHandles {
        param_t min;
        param_t trim;
        param_t max;
        param_t dz;
    };

    struct ParamValues {
        float min;
        float trim;
        float max;
        float dz;
    };

    ParamHandles _param_handles[input_rc_s::RC_INPUT_MAX_CHANNELS];
    ParamValues  _param_values[input_rc_s::RC_INPUT_MAX_CHANNELS];

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<params_id::COM_RC_IN_MODE>)_param_com_rc_in_mode)
};
