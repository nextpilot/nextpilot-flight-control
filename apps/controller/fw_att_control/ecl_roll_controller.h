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
 * @file ecl_roll_controller.h
 * Definition of a simple orthogonal roll PID controller.
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author Thomas Gubler <thomasgubler@gmail.com>
 *
 * Acknowledgements:
 *
 *   The control design is based on a design
 *   by Paul Riseborough and Andrew Tridgell, 2013,
 *   which in turn is based on initial work of
 *   Jonathan Challinger, 2012.
 */

#ifndef ECL_ROLL_CONTROLLER_H
#define ECL_ROLL_CONTROLLER_H

#include "ecl_controller.h"

class ECL_RollController : public ECL_Controller {
public:
    ECL_RollController()  = default;
    ~ECL_RollController() = default;

    /**
     * @brief Calculates both euler and body roll rate setpoints.
     *
     * @param dt Time step [s]
     * @param ctrl_data Various control inputs (attitude, body rates, attitdue stepoints, euler rate setpoints, current speeed)
     * @return Roll body rate setpoint [rad/s]
     */
    float control_attitude(const float dt, const ECL_ControlData &ctl_data) override;
};

#endif // ECL_ROLL_CONTROLLER_H
