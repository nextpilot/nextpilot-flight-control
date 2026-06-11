/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2026 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "../Common.hpp"

class FailureDetectorChecks : public HealthAndArmingCheckBase {
public:
    FailureDetectorChecks()  = default;
    ~FailureDetectorChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
};
