/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "offboardCheck.hpp"

using namespace time_literals;

void OffboardChecks::checkAndReport(const Context &context, Report &reporter) {
    reporter.failsafeFlags().offboard_control_signal_lost = true;

    offboard_control_mode_s offboard_control_mode;

    if (_offboard_control_mode_sub.copy(&offboard_control_mode)) {
        bool data_is_recent = hrt_absolute_time() < offboard_control_mode.timestamp + static_cast<hrt_abstime>(_param_com_of_loss_t.get() * 1_s);

        bool offboard_available = (offboard_control_mode.position || offboard_control_mode.velocity || offboard_control_mode.acceleration || offboard_control_mode.attitude || offboard_control_mode.body_rate || offboard_control_mode.actuator) && data_is_recent;

        if (offboard_control_mode.position && reporter.failsafeFlags().local_position_invalid) {
            offboard_available = false;

        } else if (offboard_control_mode.velocity && reporter.failsafeFlags().local_velocity_invalid) {
            offboard_available = false;

        } else if (offboard_control_mode.acceleration && reporter.failsafeFlags().local_velocity_invalid) {
            // OFFBOARD acceleration handled by position controller
            offboard_available = false;
        }

        // This is a mode requirement, no need to report
        reporter.failsafeFlags().offboard_control_signal_lost = !offboard_available;
    }
}
