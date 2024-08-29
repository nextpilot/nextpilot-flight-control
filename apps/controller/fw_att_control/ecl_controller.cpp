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
 * @file ecl_controller.cpp
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

#include "ecl_controller.h"

#include <stdio.h>
#include <mathlib/mathlib.h>

ECL_Controller::ECL_Controller() :
    _last_run(0),
    _tc(0.1f),
    _k_p(0.0f),
    _k_i(0.0f),
    _k_ff(0.0f),
    _integrator_max(0.0f),
    _max_rate(0.0f),
    _last_output(0.0f),
    _integrator(0.0f),
    _euler_rate_setpoint(0.0f),
    _body_rate_setpoint(0.0f) {
}

void ECL_Controller::reset_integrator() {
    _integrator = 0.0f;
}

void ECL_Controller::set_time_constant(float time_constant) {
    if (time_constant > 0.1f && time_constant < 3.0f) {
        _tc = time_constant;
    }
}

void ECL_Controller::set_k_p(float k_p) {
    _k_p = k_p;
}

void ECL_Controller::set_k_i(float k_i) {
    _k_i = k_i;
}

void ECL_Controller::set_k_ff(float k_ff) {
    _k_ff = k_ff;
}

void ECL_Controller::set_integrator_max(float max) {
    _integrator_max = max;
}

void ECL_Controller::set_max_rate(float max_rate) {
    _max_rate = max_rate;
}

float ECL_Controller::get_euler_rate_setpoint() {
    return _euler_rate_setpoint;
}

float ECL_Controller::get_body_rate_setpoint() {
    return _body_rate_setpoint;
}

float ECL_Controller::get_integrator() {
    return _integrator;
}
