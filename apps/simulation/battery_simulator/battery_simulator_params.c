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
 * Simulator Battery drain interval
 *
 * @min 1
 * @max 86400
 * @increment 1
 * @unit s
 *
 * @group SITL
 */
PARAM_DEFINE_FLOAT(SIM_BAT_DRAIN, 60);

/**
 * Simulator Battery minimal percentage.
 *
 * Can be used to alter the battery level during SITL- or HITL-simulation on the fly.
 * Particularly useful for testing different low-battery behaviour.
 *
 * @min 0
 * @max 100
 * @increment 0.1
 * @unit %
 *
 * @group SITL
 */
PARAM_DEFINE_FLOAT(SIM_BAT_MIN_PCT, 50.0f);
