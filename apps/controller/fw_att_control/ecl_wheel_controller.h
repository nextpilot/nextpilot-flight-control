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
 * @file ecl_wheel_controller.h
 * Definition of a simple orthogonal coordinated turn yaw PID controller.
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author Thomas Gubler <thomasgubler@gmail.com>
 * @author Andreas Antener <andreas@uaventure.com>
 *
 * Acknowledgements:
 *
 *   The control design is based on a design
 *   by Paul Riseborough and Andrew Tridgell, 2013,
 *   which in turn is based on initial work of
 *   Jonathan Challinger, 2012.
 */
#ifndef ECL_HEADING_CONTROLLER_H
#define ECL_HEADING_CONTROLLER_H

#include "ecl_controller.h"

class ECL_WheelController : public ECL_Controller {
public:
    ECL_WheelController()  = default;
    ~ECL_WheelController() = default;

    /**
     * @brief Calculates wheel body rate setpoint.
     *
     * @param dt Time step [s]
     * @param ctrl_data Various control inputs (attitude, body rates, attitdue stepoints, euler rate setpoints, current speeed)
     * @return Wheel body rate setpoint [rad/s]
     */
    float control_attitude(const float dt, const ECL_ControlData &ctl_data) override;

    float control_bodyrate(const float dt, const ECL_ControlData &ctl_data);

    float control_euler_rate(const float dt, const ECL_ControlData &ctl_data, float bodyrate_ff) {
        (void)ctl_data;
        return 0;
    }
};

#endif // ECL_HEADING_CONTROLLER_H
