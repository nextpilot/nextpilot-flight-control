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
 * @file motor_params.c
 *
 * Parameters for motors.
 *
 */


/**
 * Minimum motor rise time (slew rate limit).
 *
 * Minimum time allowed for the motor input signal to pass through
 * a range of 1000 PWM units. A value x means that the motor signal
 * can only go from 1000 to 2000 PWM in minimum x seconds.
 *
 * Zero means that slew rate limiting is disabled.
 *
 * @min 0.0
 * @unit s/(1000*PWM)
 * @group PWM Outputs
 */
PARAM_DEFINE_FLOAT(MOT_SLEW_MAX, 0.0f);

/**
 * Thrust to motor control signal model parameter
 *
 * Parameter used to model the nonlinear relationship between
 * motor control signal (e.g. PWM) and static thrust.
 *
 * The model is: rel_thrust = factor * rel_signal^2 + (1-factor) * rel_signal,
 * where rel_thrust is the normalized thrust between 0 and 1, and
 * rel_signal is the relative motor control signal between 0 and 1.
 *
 * @min 0.0
 * @max 1.0
 * @decimal 1
 * @increment 0.1
 * @group PWM Outputs
 */
PARAM_DEFINE_FLOAT(THR_MDL_FAC, 0.0f);
