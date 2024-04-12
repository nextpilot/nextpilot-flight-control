/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * Helper class to check the state_reset_status member of the ekf object.
 * Used for checking if state resets are logged correctly.
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */

#ifndef EKF_RESET_LOGGING_CHECKER_H
#define EKF_RESET_LOGGING_CHECKER_H

#include <memory>
#include "EKF/ekf.h"

class ResetLoggingChecker {
public:
    ResetLoggingChecker(std::shared_ptr<Ekf> ekf) :
        _ekf(ekf) {
    }

    // call immediately before state reset
    void capturePreResetState();

    // call immediately after state reset
    void capturePostResetState();

    bool isVelocityDeltaLoggedCorrectly(float accuracy);

    bool isHorizontalVelocityResetCounterIncreasedBy(int offset);

    bool isVerticalVelocityResetCounterIncreasedBy(int offset);

    bool isPositionDeltaLoggedCorrectly(float accuracy);

    bool isHorizontalPositionResetCounterIncreasedBy(int offset);

    bool isVerticalPositionResetCounterIncreasedBy(int offset);

private:
    std::shared_ptr<Ekf> _ekf;

    Vector3f velocity_before_reset;
    Vector3f position_before_reset;
    int      horz_vel_reset_counter_before_reset;
    int      vert_vel_reset_counter_before_reset;
    int      horz_pos_reset_counter_before_reset;
    int      vert_pos_reset_counter_before_reset;

    Vector3f velocity_after_reset;
    Vector3f position_after_reset;
    int      horz_vel_reset_counter_after_reset;
    int      vert_vel_reset_counter_after_reset;
    int      horz_pos_reset_counter_after_reset;
    int      vert_pos_reset_counter_after_reset;

    Vector3f logged_delta_velocity;
    Vector3f logged_delta_position;
};
#endif // !EKF_RESET_LOGGING_CHECKER_H
