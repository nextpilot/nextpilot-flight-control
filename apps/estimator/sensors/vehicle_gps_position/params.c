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
 * Multi GPS Blending Control Mask.
 *
 * Set bits in the following positions to set which GPS accuracy metrics will be used to calculate the blending weight. Set to zero to disable and always used first GPS instance.
 * 0 : Set to true to use speed accuracy
 * 1 : Set to true to use horizontal position accuracy
 * 2 : Set to true to use vertical position accuracy
 *
 * @group Sensors
 * @min 0
 * @max 7
 * @bit 0 use speed accuracy
 * @bit 1 use hpos accuracy
 * @bit 2 use vpos accuracy
 */
PARAM_DEFINE_INT32(SENS_GPS_MASK, 7);

/**
 * Multi GPS Blending Time Constant
 *
 * Sets the longest time constant that will be applied to the calculation of GPS position and height offsets used to correct data from multiple GPS data for steady state position differences.
 *
 *
 * @group Sensors
 * @min 1.0
 * @max 100.0
 * @unit s
 * @decimal 1
 */
PARAM_DEFINE_FLOAT(SENS_GPS_TAU, 10.0f);

/**
 * Multi GPS primary instance
 *
 * When no blending is active, this defines the preferred GPS receiver instance.
 * The GPS selection logic waits until the primary receiver is available to
 * send data to the EKF even if a secondary instance is already available.
 * The secondary instance is then only used if the primary one times out.
 *
 * To have an equal priority of all the instances, set this parameter to -1 and
 * the best receiver will be used.
 *
 * This parameter has no effect if blending is active.
 *
 * @group Sensors
 * @min -1
 * @max 1
 */
PARAM_DEFINE_INT32(SENS_GPS_PRIME, 0);
