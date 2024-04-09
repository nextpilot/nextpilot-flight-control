/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "homePositionCheck.hpp"

void HomePositionChecks::checkAndReport(const Context &context, Report &reporter) {
    home_position_s home_position;

    if (_home_position_sub.copy(&home_position)) {
        reporter.failsafeFlags().home_position_invalid = !home_position.valid_alt || !home_position.valid_hpos;

    } else {
        reporter.failsafeFlags().home_position_invalid = true;
    }

    // No need to report, as it's a mode requirement
}
