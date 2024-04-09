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
 * @file launchdetection_params.c
 *
 * Parameters for launchdetection
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 */

/**
 * Trigger acceleration threshold
 *
 * Launch is detected when acceleration in body forward direction is above FW_LAUN_AC_THLD for FW_LAUN_AC_T seconds.
 *
 * @unit m/s^2
 * @min 0
 * @decimal 1
 * @increment 0.5
 * @group FW Launch detection
 */
PARAM_DEFINE_FLOAT(FW_LAUN_AC_THLD, 30.0f);

/**
 * Trigger time
 *
 * Launch is detected when acceleration in body forward direction is above FW_LAUN_AC_THLD for FW_LAUN_AC_T seconds.
 *
 * @unit s
 * @min 0.0
 * @max 5.0
 * @decimal 2
 * @increment 0.05
 * @group FW Launch detection
 */
PARAM_DEFINE_FLOAT(FW_LAUN_AC_T, 0.05f);

/**
 * Motor delay
 *
 * Start the motor(s) this amount of seconds after launch is detected.
 *
 * @unit s
 * @min 0.0
 * @max 10.0
 * @decimal 1
 * @increment 0.5
 * @group FW Launch detection
 */
PARAM_DEFINE_FLOAT(FW_LAUN_MOT_DEL, 0.0f);
