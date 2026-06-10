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
 * @file collisionprevention_params.c
 *
 * Parameters defined by the collisionprevention lib.
 *
 * @author Tanja Baumann <tanja@auterion.com>
 */

/**
 * Minimum distance the vehicle should keep to all obstacles
 *
 * Only used in Position mode. Collision avoidance is disabled by setting this parameter to a negative value
 *
 * @min -1
 * @max 15
 * @unit m
 * @group Multicopter Position Control
 */
PARAM_DEFINE_FLOAT(CP_DIST, -1.0f);

/**
 * Average delay of the range sensor message plus the tracking delay of the position controller in seconds
 *
 * Only used in Position mode.
 *
 * @min 0
 * @max 1
 * @unit s
 * @group Multicopter Position Control
 */
PARAM_DEFINE_FLOAT(CP_DELAY, 0.4f);

/**
 * Angle left/right from the commanded setpoint by which the collision prevention algorithm can choose to change the setpoint direction
 *
 * Only used in Position mode.
 *
 * @min 0
 * @max 90
 * @unit deg
 * @group Multicopter Position Control
 */
PARAM_DEFINE_FLOAT(CP_GUIDE_ANG, 30.f);

/**
 * Boolean to allow moving into directions where there is no sensor data (outside FOV)
 *
 * Only used in Position mode.
 *
 * @boolean
 * @group Multicopter Position Control
 */
PARAM_DEFINE_INT32(CP_GO_NO_DATA, 0);
