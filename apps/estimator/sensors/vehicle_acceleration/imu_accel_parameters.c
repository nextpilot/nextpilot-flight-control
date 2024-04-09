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
 * Low pass filter cutoff frequency for accel
 *
 * The cutoff frequency for the 2nd order butterworth filter on the primary accelerometer.
 * This only affects the signal sent to the controllers, not the estimators. 0 disables the filter.
 *
 * @min 0
 * @max 1000
 * @unit Hz
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(IMU_ACCEL_CUTOFF, 30.0f);
