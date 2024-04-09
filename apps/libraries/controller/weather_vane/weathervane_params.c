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
 * @file weathervane_params.c
 *
 * Parameters defined by the weathervane lib.
 *
 * @author Roman Bapst <roman@auterion.com>
 */

/**
 * Enable weathervane.
 *
 * @boolean
 * @group Multicopter Position Control
 */
PARAM_DEFINE_INT32(WV_EN, 0);

/**
 * Weather-vane roll angle to yawrate.
 *
 * The desired gain to convert roll sp into yaw rate sp.
 *
 * @min 0.0
 * @max 3.0
 * @unit Hz
 * @increment 0.01
 * @decimal 3
 * @group VTOL Attitude Control
 */
PARAM_DEFINE_FLOAT(WV_GAIN, 1.0f);

/**
 * Minimum roll angle setpoint for weathervane controller to demand a yaw-rate.
 *
 * @min 0
 * @max 5
 * @unit deg
 * @group Multicopter Position Control
 */
PARAM_DEFINE_FLOAT(WV_ROLL_MIN, 1.0f);

/**
 * Maximum yawrate the weathervane controller is allowed to demand.
 *
 * @min 0
 * @max 120
 * @unit deg/s
 * @group Multicopter Position Control
 */
PARAM_DEFINE_FLOAT(WV_YRATE_MAX, 90.0f);
