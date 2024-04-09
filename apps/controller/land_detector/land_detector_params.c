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
 * Total flight time in microseconds
 *
 * Total flight time of this autopilot. Higher 32 bits of the value.
 * Flight time in microseconds = (LND_FLIGHT_T_HI << 32) | LND_FLIGHT_T_LO.
 *
 * @min 0
 * @volatile
 * @category system
 * @group Land Detector
 *
 */
PARAM_DEFINE_INT32(LND_FLIGHT_T_HI, 0);

/**
 * Total flight time in microseconds
 *
 * Total flight time of this autopilot. Lower 32 bits of the value.
 * Flight time in microseconds = (LND_FLIGHT_T_HI << 32) | LND_FLIGHT_T_LO.
 *
 * @min 0
 * @volatile
 * @category system
 * @group Land Detector
 *
 */
PARAM_DEFINE_INT32(LND_FLIGHT_T_LO, 0);
