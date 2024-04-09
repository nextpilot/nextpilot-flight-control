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
 * QNH for barometer
 *
 * @min 500
 * @max 1500
 * @group Sensors
 * @unit hPa
 */
PARAM_DEFINE_FLOAT(SENS_BARO_QNH, 1013.25f);

/**
 * Baro max rate.
 *
 * Barometric air data maximum publication rate. This is an upper bound,
 * actual barometric data rate is still dependent on the sensor.
 *
 * @min 1
 * @max 200
 * @group Sensors
 * @unit Hz
 */
PARAM_DEFINE_FLOAT(SENS_BARO_RATE, 20.0f);
