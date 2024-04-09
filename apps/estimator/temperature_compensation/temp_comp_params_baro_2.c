/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Barometer 2 */

/**
 * ID of Barometer that the calibration is for.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_INT32(TC_B2_ID, 0);

/**
 * Barometer offset temperature ^5 polynomial coefficient.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_X5, 0.0f);

/**
 * Barometer offset temperature ^4 polynomial coefficient.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_X4, 0.0f);

/**
 * Barometer offset temperature ^3 polynomial coefficient.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_X3, 0.0f);

/**
 * Barometer offset temperature ^2 polynomial coefficient.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_X2, 0.0f);

/**
 * Barometer offset temperature ^1 polynomial coefficients.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_X1, 0.0f);

/**
 * Barometer offset temperature ^0 polynomial coefficient.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_X0, 0.0f);

/**
 * Barometer calibration reference temperature.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_TREF, 40.0f);

/**
 * Barometer calibration minimum temperature.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_TMIN, 5.0f);

/**
 * Barometer calibration maximum temperature.
 *
 * @group Thermal Compensation
 * @category system
 */
PARAM_DEFINE_FLOAT(TC_B2_TMAX, 75.0f);
