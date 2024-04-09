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
 * @file geofence_params.c
 *
 * Parameters for geofence
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 */

/*
 * Geofence parameters, accessible via MAVLink
 */

/**
 * Geofence violation action.
 *
 * Note: Setting this value to 4 enables flight termination,
 * which will kill the vehicle on violation of the fence.
 *
 * @min 0
 * @max 5
 * @value 0 None
 * @value 1 Warning
 * @value 2 Hold mode
 * @value 3 Return mode
 * @value 4 Terminate
 * @value 5 Land mode
 * @group Geofence
 */
PARAM_DEFINE_INT32(GF_ACTION, 2);

/**
 * Geofence altitude mode
 *
 * Select which altitude (AMSL) source should be used for geofence calculations.
 *
 * @min 0
 * @max 1
 * @value 0 Autopilot estimator global position altitude (GPS)
 * @value 1 Raw barometer altitude (assuming standard atmospheric pressure)
 * @group Geofence
 */
PARAM_DEFINE_INT32(GF_ALTMODE, 0);

/**
 * Geofence source
 *
 * Select which position source should be used. Selecting GPS instead of global position makes sure that there is
 * no dependence on the position estimator
 * 0 = global position, 1 = GPS
 *
 * @min 0
 * @max 1
 * @value 0 GPOS
 * @value 1 GPS
 * @group Geofence
 */
PARAM_DEFINE_INT32(GF_SOURCE, 0);

/**
 * Geofence counter limit
 *
 * Set how many subsequent position measurements outside of the fence are needed before geofence violation is triggered
 *
 * @min -1
 * @max 10
 * @increment 1
 * @group Geofence
 */
PARAM_DEFINE_INT32(GF_COUNT, -1);

/**
 * Max horizontal distance in meters.
 *
 * Maximum horizontal distance in meters the vehicle can be from home before triggering a geofence action. Disabled if 0.
 *
 * @unit m
 * @min 0
 * @max 10000
 * @increment 1
 * @group Geofence
 */
PARAM_DEFINE_FLOAT(GF_MAX_HOR_DIST, 0);

/**
 * Max vertical distance in meters.
 *
 * Maximum vertical distance in meters the vehicle can be from home before triggering a geofence action. Disabled if 0.
 *
 * @unit m
 * @min 0
 * @max 10000
 * @increment 1
 * @group Geofence
 */
PARAM_DEFINE_FLOAT(GF_MAX_VER_DIST, 0);

/**
 * [EXPERIMENTAL] Use Pre-emptive geofence triggering
 *
 * WARNING: This experimental feature may cause flyaways. Use at your own risk.
 *
 * Predict the motion of the vehicle and trigger the breach if it is determined that the current trajectory
 * would result in a breach happening before the vehicle can make evasive maneuvers.
 * The vehicle is then re-routed to a safe hold position (stop for multirotor, loiter for fixed wing).
 *
 * @boolean
 * @group Geofence
 */
PARAM_DEFINE_INT32(GF_PREDICT, 0);
