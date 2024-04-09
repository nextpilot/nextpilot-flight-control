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
 * Enable simulated barometer sensor instance
 *
 * @reboot_required true
 * @min 0
 * @max 1
 * @group Sensors
 * @value 0 Disabled
 * @value 1 Enabled
 */
PARAM_DEFINE_INT32(SENS_EN_BAROSIM, 0);

/**
 * simulated barometer pressure offset
 *
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_BARO_OFF_P, 0.0f);

/**
 * simulated barometer temperature offset
 *
 * @group Simulator
 * @unit celcius
 */
PARAM_DEFINE_FLOAT(SIM_BARO_OFF_T, 0.0f);
