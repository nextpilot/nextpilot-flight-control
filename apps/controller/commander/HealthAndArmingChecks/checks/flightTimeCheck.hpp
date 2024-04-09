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

class FlightTimeChecks : public HealthAndArmingCheckBase {
public:
    FlightTimeChecks()  = default;
    ~FlightTimeChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
    int _last_flight_time_warning_sec{-1};

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<px4::params::COM_FLT_TIME_MAX>)_param_com_flt_time_max)
};
