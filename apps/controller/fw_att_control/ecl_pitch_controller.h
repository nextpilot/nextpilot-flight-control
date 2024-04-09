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
 * @file ecl_pitch_controller.h
 * Definition of a simple orthogonal pitch PID controller.
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

#ifndef ECL_PITCH_CONTROLLER_H
#define ECL_PITCH_CONTROLLER_H

#include <mathlib/mathlib.h>

#include "ecl_controller.h"

class ECL_PitchController : public ECL_Controller {
public:
    ECL_PitchController()  = default;
    ~ECL_PitchController() = default;

    /**
     * @brief Calculates both euler and body pitch rate setpoints.
     *
     * @param dt Time step [s]
     * @param ctrl_data Various control inputs (attitude, body rates, attitdue stepoints, euler rate setpoints, current speeed)
     * @return Pitch body rate setpoint [rad/s]
     */
    float control_attitude(const float dt, const ECL_ControlData &ctl_data) override;

    /* Additional Setters */
    void set_max_rate_pos(float max_rate_pos) {
        _max_rate = max_rate_pos;
    }

    void set_max_rate_neg(float max_rate_neg) {
        _max_rate_neg = max_rate_neg;
    }

protected:
    float _max_rate_neg{0.0f};
};

#endif // ECL_PITCH_CONTROLLER_H
