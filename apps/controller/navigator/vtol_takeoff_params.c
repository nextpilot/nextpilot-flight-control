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
 * @file VTOLTakeoff_params.c
 *
 * Parameters for the VTOL takeoff navigation mode.
 *
 */

/**
 * VTOL Takeoff relative loiter altitude.
 *
 * Altitude relative to home at which vehicle will loiter after front transition.
 *
 * @unit m
 * @min 20
 * @max 300
 * @decimal 1
 * @increment 1
 * @group VTOL Takeoff
 */
PARAM_DEFINE_FLOAT(VTO_LOITER_ALT, 80);
