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
 * Enable online mag bias calibration
 *
 * This enables continuous calibration of the magnetometers
 * before takeoff using gyro data.
 *
 * @boolean
 * @reboot_required true
 * @group Magnetometer Bias Estimator
 */
PARAM_DEFINE_INT32(MBE_ENABLE, 1);

/**
 * Mag bias estimator learning gain
 *
 * Increase to make the estimator more responsive
 * Decrease to make the estimator more robust to noise
 *
 * @min 0.1
 * @max 100
 * @increment 0.1
 * @decimal 1
 * @group Magnetometer Bias Estimator
 */
PARAM_DEFINE_FLOAT(MBE_LEARN_GAIN, 18.f);
