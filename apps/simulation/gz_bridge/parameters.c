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
 * Simulator Gazebo bridge enable
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(SIM_GZ_EN, 0);

/**
 * simulator origin latitude
 *
 * @unit deg
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_GZ_HOME_LAT, 47.397742f);

/**
 * simulator origin longitude
 *
 * @unit deg
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_GZ_HOME_LON, 8.545594);

/**
 * simulator origin altitude
 *
 * @unit m
 * @group Simulator
 */
PARAM_DEFINE_FLOAT(SIM_GZ_HOME_ALT, 488.0);
