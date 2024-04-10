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
 * @file ecl_controller.h
 * Definition of base class for other controllers
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

#pragma once

#include <hrtimer.h>
// #include <px4_log.h>

struct ECL_ControlData {
    float roll;
    float pitch;
    float yaw;
    float body_z_rate;
    float roll_setpoint;
    float pitch_setpoint;
    float yaw_setpoint;
    float euler_pitch_rate_setpoint;
    float euler_yaw_rate_setpoint;
    float airspeed_constrained;
    float groundspeed;
    float groundspeed_scaler;
};

class ECL_Controller {
public:
    ECL_Controller();
    virtual ~ECL_Controller() = default;

    /**
     * @brief Calculates both euler and body rate setpoints. Has different implementations for all body axes.
     *
     * @param dt Time step [s]
     * @param ctrl_data Various control inputs (attitude, body rates, attitdue stepoints, euler rate setpoints, current speeed)
     * @return Body rate setpoint [rad/s]
     */
    virtual float control_attitude(const float dt, const ECL_ControlData &ctl_data) = 0;

    /* Setters */
    void set_time_constant(float time_constant);
    void set_k_p(float k_p);
    void set_k_i(float k_i);
    void set_k_ff(float k_ff);
    void set_integrator_max(float max);
    void set_max_rate(float max_rate);

    /* Getters */
    float get_euler_rate_setpoint();
    float get_body_rate_setpoint();
    float get_integrator();

    void reset_integrator();

protected:
    uint64_t _last_run;
    float    _tc;
    float    _k_p;
    float    _k_i;
    float    _k_ff;
    float    _integrator_max;
    float    _max_rate;
    float    _last_output;
    float    _integrator;
    float    _euler_rate_setpoint;
    float    _body_rate_setpoint;
};
