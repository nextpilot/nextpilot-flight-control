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
 * @file follow_target_params.c
 *
 * Parameters for follow target mode
 *
 * @author Jimmy Johnson <catch22@fastmail.net>
 * @author Junwoo Hwang <junwoo091400@gmail.com>
 */

/**
 * Responsiveness to target movement in Target Estimator
 *
 * lower values increase the responsiveness to changing position, but also ignore less noise
 *
 * @min 0.0
 * @max 1.0
 * @decimal 2
 * @group Follow target
 */
PARAM_DEFINE_FLOAT(FLW_TGT_RS, 0.1f);

/**
 * Follow target height
 *
 * Following height above the target
 *
 * @unit m
 * @min 8.0
 * @group Follow target
 */
PARAM_DEFINE_FLOAT(FLW_TGT_HT, 8.0f);

/**
 * Distance to follow target from
 *
 * The distance in meters to follow the target at
 *
 * @unit m
 * @min 1.0
 * @group Follow target
 */
PARAM_DEFINE_FLOAT(FLW_TGT_DST, 8.0f);

/**
 * Follow Angle setting in degrees
 *
 * Angle to follow the target from. 0.0 Equals straight in front of the target's
 * course (direction of motion) and the angle increases in clockwise direction,
 * meaning Right-side would be 90.0 degrees while Left-side is -90.0 degrees
 *
 * Note: When the user force sets the angle out of the min/max range, it will be
 * wrapped (e.g. 480 -> 120) in the range to gracefully handle the out of range.
 *
 * @min -180.0
 * @max 180.0
 * @group Follow target
 */
PARAM_DEFINE_FLOAT(FLW_TGT_FA, 180.0f);

/**
 * Altitude control mode
 *
 * Maintain altitude or track target's altitude. When maintaining the altitude,
 * the drone can crash into terrain when the target moves uphill. When tracking
 * the target's altitude, the follow altitude FLW_TGT_HT should be high enough
 * to prevent terrain collisions due to GPS inaccuracies of the target.
 *
 * @value 0 2D Tracking: Maintain constant altitude relative to home and track XY position only
 * @value 1 2D + Terrain: Maintain constant altitude relative to terrain below and track XY position
 * @value 2 3D Tracking: Track target's altitude (be aware that GPS altitude bias usually makes this useless)
 * @group Follow target
 */
PARAM_DEFINE_INT32(FLW_TGT_ALT_M, 0);

/**
 * Maximum tangential velocity setting for generating the follow orbit trajectory
 *
 * This is the maximum tangential velocity the drone will circle around the target whenever
 * an orbit angle setpoint changes. Higher value means more aggressive follow behavior.
 *
 * @min 0.0
 * @max 20.0
 * @decimal 1
 * @group Follow target
 */
PARAM_DEFINE_FLOAT(FLW_TGT_MAX_VEL, 5.0f);
