/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Accelerometer 3 */

/**
 * ID of Accelerometer that the calibration is for.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_INT32(TC_A3_ID, 0);

/**
 * Accelerometer offset temperature ^3 polynomial coefficient - X axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X3_0, 0.0f);

/**
 * Accelerometer offset temperature ^3 polynomial coefficient - Y axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X3_1, 0.0f);

/**
 * Accelerometer offset temperature ^3 polynomial coefficient - Z axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X3_2, 0.0f);

/**
 * Accelerometer offset temperature ^2 polynomial coefficient - X axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X2_0, 0.0f);

/**
 * Accelerometer offset temperature ^2 polynomial coefficient - Y axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X2_1, 0.0f);

/**
 * Accelerometer offset temperature ^2 polynomial coefficient - Z axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X2_2, 0.0f);

/**
 * Accelerometer offset temperature ^1 polynomial coefficient - X axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X1_0, 0.0f);

/**
 * Accelerometer offset temperature ^1 polynomial coefficient - Y axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X1_1, 0.0f);

/**
 * Accelerometer offset temperature ^1 polynomial coefficient - Z axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X1_2, 0.0f);

/**
 * Accelerometer offset temperature ^0 polynomial coefficient - X axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X0_0, 0.0f);

/**
 * Accelerometer offset temperature ^0 polynomial coefficient - Y axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X0_1, 0.0f);

/**
 * Accelerometer offset temperature ^0 polynomial coefficient - Z axis.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_X0_2, 0.0f);

/**
 * Accelerometer calibration reference temperature.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_TREF, 25.0f);

/**
 * Accelerometer calibration minimum temperature.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_TMIN, 0.0f);

/**
 * Accelerometer calibration maximum temperature.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_A3_TMAX, 100.0f);
