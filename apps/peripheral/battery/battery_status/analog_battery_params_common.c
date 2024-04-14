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
 * Offset in volt as seen by the ADC input of the current sensor.
 *
 * This offset will be subtracted before calculating the battery
 * current based on the voltage.
 *
 * @group Battery Calibration
 * @decimal 8
 */
PARAM_DEFINE_FLOAT(BAT_V_OFFS_CURR, 0.0);
