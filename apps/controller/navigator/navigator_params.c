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
 * @file navigator_params.c
 *
 * Parameters for navigator in general
 *
 * @author Julian Oes <julian@px4.io>
 * @author Thomas Gubler <thomas@px4.io>
 */

/**
 * Loiter radius (FW only)
 *
 * Default value of loiter radius for missions, Hold mode, Return mode, etc. (fixedwing only).
 *
 * @unit m
 * @min 25
 * @max 1000
 * @decimal 1
 * @increment 0.5
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_LOITER_RAD, 80.0f);

/**
 * Acceptance Radius
 *
 * Default acceptance radius, overridden by acceptance radius of waypoint if set.
 * For fixed wing the npfg switch distance is used for horizontal acceptance.
 *
 * @unit m
 * @min 0.05
 * @max 200.0
 * @decimal 1
 * @increment 0.5
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_ACC_RAD, 10.0f);

/**
 * FW Altitude Acceptance Radius
 *
 * Acceptance radius for fixedwing altitude.
 *
 * @unit m
 * @min 0.05
 * @max 200.0
 * @decimal 1
 * @increment 0.5
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_FW_ALT_RAD, 10.0f);

/**
 * FW Altitude Acceptance Radius before a landing
 *
 * Altitude acceptance used for the last waypoint before a fixed-wing landing. This is usually smaller
 * than the standard vertical acceptance because close to the ground higher accuracy is required.
 *
 * @unit m
 * @min 0.05
 * @max 200.0
 * @decimal 1
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_FW_ALTL_RAD, 5.0f);

/**
 * MC Altitude Acceptance Radius
 *
 * Acceptance radius for multicopter altitude.
 *
 * @unit m
 * @min 0.05
 * @max 200.0
 * @decimal 1
 * @increment 0.5
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_MC_ALT_RAD, 0.8f);

/**
 * Set traffic avoidance mode
 *
 * Enabling this will allow the system to respond
 * to transponder data from e.g. ADSB transponders
 *
 * @value 0 Disabled
 * @value 1 Warn only
 * @value 2 Return mode
 * @value 3 Land mode
 * @value 4 Position Hold mode
 *
 * @group Mission
 */
PARAM_DEFINE_INT32(NAV_TRAFF_AVOID, 1);

/**
 * Set NAV TRAFFIC AVOID horizontal distance
 *
 * Defines a crosstrack horizontal distance
 *
 * @unit m
 * @min 500
 *
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_TRAFF_A_HOR, 500);

/**
 * Set NAV TRAFFIC AVOID vertical distance
 *
 *
 * @unit m
 * @min 10
 * @max 500
 *
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_TRAFF_A_VER, 500);

/**
 * Estimated time until collision
 *
 * Minimum acceptable time until collsion.
 * Assumes constant speed over 3d distance.
 *
 * @unit s
 * @min 1
 * @max 900000000
 * @group Mission
 */
PARAM_DEFINE_INT32(NAV_TRAFF_COLL_T, 60);

/**
 * Force VTOL mode takeoff and land
 *
 * @boolean
 * @group Mission
 */
PARAM_DEFINE_INT32(NAV_FORCE_VT, 1);

/**
 * Minimum Loiter altitude
 *
 * This is the minimum altitude above Home the system will always obey in Loiter (Hold) mode if switched into this
 * mode without specifying an altitude (e.g. through Loiter switch on RC).
 * Doesn't affect Loiters that are part of Missions or that are entered through a reposition setpoint ("Go to").
 * Set to a negative value to disable.
 *
 * @unit m
 * @min -1
 * @decimal 1
 * @increment 0.5
 * @group Mission
 */
PARAM_DEFINE_FLOAT(NAV_MIN_LTR_ALT, -1.f);
