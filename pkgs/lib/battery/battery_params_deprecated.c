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
 * @file battery_params_deprecated.c
 *
 * Defines the deprcated single battery configuration which are temporarily kept for backwards compatibility with QGC.
 */

/**
 * This parameter is deprecated. Please use BAT1_V_EMPTY instead.
 *
 * @group Battery Calibration
 * @category system
 */
PARAM_DEFINE_FLOAT(BAT_V_EMPTY, 3.6f);

/**
 * This parameter is deprecated. Please use BAT1_V_CHARGED instead.
 *
 * @group Battery Calibration
 * @category system
 */
PARAM_DEFINE_FLOAT(BAT_V_CHARGED, 4.05f);

/**
 * This parameter is deprecated. Please use BAT1_V_LOAD_DROP instead.
 *
 * @group Battery Calibration
 * @category system
 */
PARAM_DEFINE_FLOAT(BAT_V_LOAD_DROP, 0.3f);

/**
 * This parameter is deprecated. Please use BAT1_N_CELLS instead.
 *
 * @group Battery Calibration
 * @category system
 */
PARAM_DEFINE_INT32(BAT_N_CELLS, 3);
