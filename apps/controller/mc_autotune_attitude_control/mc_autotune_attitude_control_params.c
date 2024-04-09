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
 * @file autotune_attitude_control_params.c
 *
 * Parameters used by the attitude auto-tuner
 *
 * @author Mathieu Bresciani <mathieu@auterion.com>
 */

/**
 * Multicopter autotune module enable
 *
 * @boolean
 * @group Autotune
 */
PARAM_DEFINE_INT32(MC_AT_EN, 0);

/**
 * Start the autotuning sequence
 *
 * WARNING: this will inject steps to the rate controller
 * and can be dangerous. Only activate if you know what you
 * are doing, and in a safe environment.
 *
 * Any motion of the remote stick will abort the signal
 * injection and reset this parameter
 * Best is to perform the identification in position or
 * hold mode.
 * Increase the amplitude of the injected signal using
 * MC_AT_SYSID_AMP for more signal/noise ratio
 *
 * @boolean
 * @group Autotune
 */
PARAM_DEFINE_INT32(MC_AT_START, 0);

/**
 * Amplitude of the injected signal
 *
 * @min 0.1
 * @max 6.0
 * @decimal 1
 * @group Autotune
 */
PARAM_DEFINE_FLOAT(MC_AT_SYSID_AMP, 0.7);

/**
 * Controls when to apply the new gains
 *
 * After the auto-tuning sequence is completed,
 * a new set of gains is available and can be applied
 * immediately or after landing.
 *
 * WARNING Applying the gains in air is dangerous as there is no
 * guarantee that those new gains will be able to stabilize
 * the drone properly.
 *
 * @value 0 Do not apply the new gains (logging only)
 * @value 1 Apply the new gains after disarm
 * @value 2 WARNING Apply the new gains in air
 * @group Autotune
 */
PARAM_DEFINE_INT32(MC_AT_APPLY, 1);

/**
 * Desired angular rate closed-loop rise time
 *
 * @min 0.01
 * @max 0.5
 * @decimal 2
 * @unit s
 * @group Autotune
 */
PARAM_DEFINE_FLOAT(MC_AT_RISE_TIME, 0.14);
