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
 * Bitfield selecting mag sides for calibration
 *
 * If set to two side calibration, only the offsets are estimated, the scale
 * calibration is left unchanged. Thus an initial six side calibration is
 * recommended.
 *
 * Bits:
 * ORIENTATION_TAIL_DOWN = 1
 * ORIENTATION_NOSE_DOWN = 2
 * ORIENTATION_LEFT = 4
 * ORIENTATION_RIGHT = 8
 * ORIENTATION_UPSIDE_DOWN = 16
 * ORIENTATION_RIGHTSIDE_UP = 32
 *
 * @min 34
 * @max 63
 * @value 34 Two side calibration
 * @value 38 Three side calibration
 * @value 63 Six side calibration
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_MAG_SIDES, 63);

/**
 * For legacy QGC support only
 *
 * Use SENS_MAG_SIDES instead
 *
 * @group Sensors
 * @category Developer
 */
PARAM_DEFINE_INT32(CAL_MAG_SIDES, 63);

/**
 * Type of magnetometer compensation
 *
 * @value 0 Disabled
 * @value 1 Throttle-based compensation
 * @value 2 Current-based compensation (battery_status instance 0)
 * @value 3 Current-based compensation (battery_status instance 1)
 *
 * @category system
 * @group Sensor Calibration
 */
PARAM_DEFINE_INT32(CAL_MAG_COMP_TYP, 0);

/**
 * Automatically set external rotations.
 *
 * During calibration attempt to automatically determine the rotation of external magnetometers.
 *
 * @boolean
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_MAG_AUTOROT, 1);

/**
 * Magnetometer max rate.
 *
 * Magnetometer data maximum publication rate. This is an upper bound,
 * actual magnetometer data rate is still dependent on the sensor.
 *
 * @min 1
 * @max 200
 * @group Sensors
 * @unit Hz
 *
 * @reboot_required true
 *
 */
PARAM_DEFINE_FLOAT(SENS_MAG_RATE, 15.0f);

/**
 * Sensors hub mag mode
 *
 * @value 0 Publish all magnetometers
 * @value 1 Publish primary magnetometer
 *
 * @category system
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_MAG_MODE, 1);

/**
 * Magnetometer auto calibration
 *
 * Automatically initialize magnetometer calibration from bias estimate if available.
 *
 * @boolean
 *
 * @category system
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_MAG_AUTOCAL, 1);
