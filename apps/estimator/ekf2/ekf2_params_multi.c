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
 * Multi-EKF IMUs
 *
 * Maximum number of IMUs to use for Multi-EKF. Set 0 to disable.
 * Requires SENS_IMU_MODE 0.
 *
 * @group EKF2
 * @reboot_required true
 * @min 0
 * @max 4
 */
PARAM_DEFINE_INT32(EKF2_MULTI_IMU, 0);

/**
 * Multi-EKF Magnetometers.
 *
 * Maximum number of magnetometers to use for Multi-EKF. Set 0 to disable.
 * Requires SENS_MAG_MODE 0.
 *
 * @group EKF2
 * @reboot_required true
 * @min 0
 * @max 4
 */
PARAM_DEFINE_INT32(EKF2_MULTI_MAG, 0);
