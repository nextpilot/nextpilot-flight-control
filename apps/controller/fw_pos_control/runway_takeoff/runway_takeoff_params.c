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
 * @file runway_takeoff_params.c
 *
 * Parameters for runway takeoff
 *
 * @author Andreas Antener <andreas@uaventure.com>
 */

/**
 * Runway takeoff with landing gear
 *
 * @boolean
 * @group Runway Takeoff
 */
PARAM_DEFINE_INT32(RWTO_TKOFF, 0);

/**
 * Specifies which heading should be held during the runway takeoff ground roll.
 *
 * 0: airframe heading when takeoff is initiated
 * 1: position control along runway direction (bearing defined from vehicle position on takeoff initiation to MAV_CMD_TAKEOFF
 *    position defined by operator)
 *
 * @value 0 Airframe
 * @value 1 Runway
 * @min 0
 * @max 1
 * @group Runway Takeoff
 */
PARAM_DEFINE_INT32(RWTO_HDG, 0);

/**
 * Max throttle during runway takeoff.
 *
 * @unit norm
 * @min 0.0
 * @max 1.0
 * @decimal 2
 * @increment 0.01
 * @group Runway Takeoff
 */
PARAM_DEFINE_FLOAT(RWTO_MAX_THR, 1.0);

/**
 * Pitch setpoint during taxi / before takeoff rotation airspeed is reached.
 *
 * A taildragger with steerable wheel might need to pitch up
 * a little to keep its wheel on the ground before airspeed
 * to takeoff is reached.
 *
 * @unit deg
 * @min -10.0
 * @max 20.0
 * @decimal 1
 * @increment 0.5
 * @group Runway Takeoff
 */
PARAM_DEFINE_FLOAT(RWTO_PSP, 0.0);

/**
 * Throttle ramp up time for runway takeoff
 *
 * @unit s
 * @min 1.0
 * @max 15.0
 * @decimal 2
 * @increment 0.1
 * @group Runway Takeoff
 */
PARAM_DEFINE_FLOAT(RWTO_RAMP_TIME, 2.0f);

/**
 * NPFG period while steering on runway
 *
 * @unit s
 * @min 1.0
 * @max 100.0
 * @decimal 1
 * @increment 0.1
 * @group Runway Takeoff
 */
PARAM_DEFINE_FLOAT(RWTO_NPFG_PERIOD, 5.0f);

/**
 * Enable use of yaw stick for nudging the wheel during runway ground roll
 *
 * This is useful when map, GNSS, or yaw errors on ground are misaligned with what the operator intends for takeoff course.
 * Particularly useful for skinny runways or if the wheel servo is a bit off trim.
 *
 * @boolean
 * @group Runway Takeoff
 */
PARAM_DEFINE_INT32(RWTO_NUDGE, 1);

/**
 * Takeoff rotation airspeed
 *
 * The calibrated airspeed threshold during the takeoff ground roll when the plane should start rotating (pitching up).
 * Must be less than the takeoff airspeed, will otherwise be capped at the takeoff airpeed (see FW_TKO_AIRSPD).
 *
 * If set <= 0.0, defaults to 0.9 * takeoff airspeed (see FW_TKO_AIRSPD)
 *
 * @unit m/s
 * @min -1.0
 * @decimal 1
 * @increment 0.1
 * @group Runway Takeoff
 */
PARAM_DEFINE_FLOAT(RWTO_ROT_AIRSPD, -1.0f);

/**
 * Takeoff rotation time
 *
 * This is the time desired to linearly ramp in takeoff pitch constraints during the takeoff rotation
 *
 * @unit s
 * @min 0.1
 * @decimal 1
 * @increment 0.1
 * @group Runway Takeoff
 */
PARAM_DEFINE_FLOAT(RWTO_ROT_TIME, 1.0f);
