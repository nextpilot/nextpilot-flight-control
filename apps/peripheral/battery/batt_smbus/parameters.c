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
 * SMBUS Smart battery driver BQ40Z50 and BQ40Z80
 *
 * @reboot_required true
 *
 * @boolean
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_EN_BATT, 0);

/**
 * Capacity/current multiplier for high-current capable SMBUS battery
 *
 * @reboot_required true
 * @decimal 1
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(BAT1_C_MULT, 1.0f);

/**
 * Battery device model
 *
 * @reboot_required true
 * @min 0
 * @max 2
 * @group Sensors
 * @value 0 AutoDetect
 * @value 1 BQ40Z50 based
 * @value 2 BQ40Z80 based
 */
PARAM_DEFINE_INT32(BAT1_SMBUS_MODEL, 0);
