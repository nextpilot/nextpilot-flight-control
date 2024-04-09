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
 * Multicopter land detection trigger time
 *
 * Total time it takes to go through all three land detection stages:
 * ground contact, maybe landed, landed
 * when all necessary conditions are constantly met.
 *
 * @unit s
 * @min 0.1
 * @max 10.0
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDMC_TRIG_TIME, 1.0f);

/**
 * Multicopter vertical velocity threshold
 *
 * Vertical velocity threshold to detect landing.
 * Has to be set lower than the expected minimal speed for landing,
 * which is either MPC_LAND_SPEED or MPC_LAND_CRWL.
 * This is enforced by an automatic check.
 *
 * @unit m/s
 * @min 0
 * @decimal 2
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDMC_Z_VEL_MAX, 0.25f);

/**
 * Multicopter max horizontal velocity
 *
 * Maximum horizontal velocity allowed in the landed state
 *
 * @unit m/s
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDMC_XY_VEL_MAX, 1.5f);

/**
 * Multicopter max rotation
 *
 * Maximum allowed angular velocity around each axis allowed in the landed state.
 *
 * @unit deg/s
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDMC_ROT_MAX, 20.0f);

/**
 * Maximum altitude for multicopters
 *
 * The system will obey this limit as a
 * hard altitude limit. This setting will
 * be consolidated with the GF_MAX_VER_DIST
 * parameter.
 * A negative value indicates no altitude limitation.
 *
 * @unit m
 * @min -1
 * @max 10000
 * @decimal 2
 * @group Land Detector
 *
 */
PARAM_DEFINE_FLOAT(LNDMC_ALT_MAX, -1.0f);

/**
 * Ground effect altitude for multicopters
 *
 * The height above ground below which ground effect creates barometric altitude errors.
 * A negative value indicates no ground effect.
 *
 * @unit m
 * @min -1
 * @decimal 2
 * @group Land Detector
 *
 */
PARAM_DEFINE_FLOAT(LNDMC_ALT_GND, 2.f);
