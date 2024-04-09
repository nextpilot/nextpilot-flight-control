/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/*
 * @file attitude_estimator_q_params.c
 *
 * Parameters for attitude estimator (quaternion based)
 *
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

/**
 * Complimentary filter accelerometer weight
 *
 * @group Attitude Q estimator
 * @min 0
 * @max 1
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(ATT_W_ACC, 0.2f);

/**
 * Complimentary filter magnetometer weight
 *
 * Set to 0 to avoid using the magnetometer.
 *
 * @group Attitude Q estimator
 * @min 0
 * @max 1
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(ATT_W_MAG, 0.1f);

/**
 * Complimentary filter external heading weight
 *
 * @group Attitude Q estimator
 * @min 0
 * @max 1
 */
PARAM_DEFINE_FLOAT(ATT_W_EXT_HDG, 0.1f);

/**
 * Complimentary filter gyroscope bias weight
 *
 * @group Attitude Q estimator
 * @min 0
 * @max 1
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(ATT_W_GYRO_BIAS, 0.1f);

/**
 * Magnetic declination, in degrees
 *
 * This parameter is not used in normal operation,
 * as the declination is looked up based on the
 * GPS coordinates of the vehicle.
 *
 * @group Attitude Q estimator
 * @unit deg
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(ATT_MAG_DECL, 0.0f);

/**
 * Automatic GPS based declination compensation
 *
 * @group Attitude Q estimator
 * @boolean
 */
PARAM_DEFINE_INT32(ATT_MAG_DECL_A, 1);

/**
 * External heading usage mode (from Motion capture/Vision)
 *
 * Set to 1 to use heading estimate from vision.
 * Set to 2 to use heading from motion capture.
 *
 * @group Attitude Q estimator
 * @value 0 None
 * @value 1 Vision
 * @value 2 Motion Capture
 * @min 0
 * @max 2
 */
PARAM_DEFINE_INT32(ATT_EXT_HDG_M, 0);

/**
 * Acceleration compensation based on GPS velocity.
 *
 * @group Attitude Q estimator
 * @boolean
 */
PARAM_DEFINE_INT32(ATT_ACC_COMP, 1);

/**
 * Gyro bias limit
 *
 * @group Attitude Q estimator
 * @unit rad/s
 * @min 0
 * @max 2
 * @decimal 3
 */
PARAM_DEFINE_FLOAT(ATT_BIAS_MAX, 0.05f);
