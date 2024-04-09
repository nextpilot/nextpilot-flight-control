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
 * Fixed-wing land detector: Max horizontal velocity threshold
 *
 * Maximum horizontal velocity allowed in the landed state.
 * A factor of 0.7 is applied in case of airspeed-less flying
 * (either because no sensor is present or sensor data got invalid in flight).
 *
 * @unit m/s
 * @min 0.5
 * @max 10
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDFW_VEL_XY_MAX, 5.0f);

/**
 * Fixed-wing land detector: Max vertiacal velocity threshold
 *
 * Maximum vertical velocity allowed in the landed state.
 *
 * @unit m/s
 * @min 0.1
 * @max 20
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDFW_VEL_Z_MAX, 1.0f);

/**
 * Fixed-wing land detector: Max horizontal acceleration
 *
 * Maximum horizontal (x,y body axes) acceleration allowed in the landed state
 *
 * @unit m/s^2
 * @min 2
 * @max 15
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDFW_XYACC_MAX, 8.0f);

/**
 * Fixed-wing land detector: Max airspeed
 *
 * Maximum airspeed allowed in the landed state
 *
 * @unit m/s
 * @min 2
 * @max 20
 * @decimal 1
 *
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDFW_AIRSPD_MAX, 6.00f);

/**
 * Fixed-wing land detection trigger time
 *
 * Time the land conditions (speeds and acceleration) have to be satisfied to detect a landing.
 *
 * @unit s
 * @min 0.1
 * @decimal 1
 *
 * @reboot_required true
 * @group Land Detector
 */
PARAM_DEFINE_FLOAT(LNDFW_TRIG_TIME, 2.f);
