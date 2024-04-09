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
 * Enable simulated magnetometer sensor instance
 *
 * @reboot_required true
 * @min 0
 * @max 1
 * @group Sensors
 * @value 0 Disabled
 * @value 1 Enabled
 */
PARAM_DEFINE_INT32(SENS_EN_MAGSIM, 0);

/**
 * simulated magnetometer X offset
 *
 * @unit gauss
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_MAG_OFFSET_X, 0.0f);

/**
 * simulated magnetometer Y offset
 *
 * @unit gauss
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_MAG_OFFSET_Y, 0.0f);

/**
 * simulated magnetometer Z offset
 *
 * @unit gauss
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_MAG_OFFSET_Z, 0.0f);
