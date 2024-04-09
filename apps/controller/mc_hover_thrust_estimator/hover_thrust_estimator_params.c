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
 * @file hover_thrust_estimator_params.c
 *
 * Parameters used by the hover thrust estimator
 *
 * @author Mathieu Bresciani <brescianimathieu@gmail.com>
 */

/**
 * Hover thrust process noise
 *
 * Reduce to make the hover thrust estimate
 * more stable, increase if the real hover thrust
 * is expected to change quickly over time.
 *
 * @decimal 4
 * @min 0.0001
 * @max 1.0
 * @unit normalized_thrust/s
 * @group Hover Thrust Estimator
 */
PARAM_DEFINE_FLOAT(HTE_HT_NOISE, 0.0036);

/**
 * Gate size for acceleration fusion
 *
 * Sets the number of standard deviations used
 * by the innovation consistency test.
 *
 * @decimal 1
 * @min 1.0
 * @max 10.0
 * @unit SD
 * @group Hover Thrust Estimator
 */
PARAM_DEFINE_FLOAT(HTE_ACC_GATE, 3.0);

/**
 * 1-sigma initial hover thrust uncertainty
 *
 * Sets the number of standard deviations used
 * by the innovation consistency test.
 *
 * @decimal 3
 * @min 0.0
 * @max 1.0
 * @unit normalized_thrust
 * @group Hover Thrust Estimator
 */
PARAM_DEFINE_FLOAT(HTE_HT_ERR_INIT, 0.1);

/**
 * Horizontal velocity threshold for sensitivity reduction
 *
 * Above this speed, the measurement noise is linearly increased
 * to reduce the sensitivity of the estimator from biased measurement.
 *
 * Set to a low value on vehicles with large lifting surfaces.
 *
 * @decimal 1
 * @min 1.0
 * @max 20.0
 * @unit m/s
 * @group Hover Thrust Estimator
 */
PARAM_DEFINE_FLOAT(HTE_VXY_THR, 10.0);

/**
 * Vertical velocity threshold for sensitivity reduction
 *
 * Above this speed, the measurement noise is linearly increased
 * to reduce the sensitivity of the estimator from biased measurement.
 *
 * Set to a low value on vehicles affected by air drag when climbing or descending.
 *
 * @decimal 1
 * @min 1.0
 * @max 10.0
 * @unit m/s
 * @group Hover Thrust Estimator
 */
PARAM_DEFINE_FLOAT(HTE_VZ_THR, 2.0);

/**
 * Max deviation from MPC_THR_HOVER
 *
 * Defines the range of the hover thrust estimate around MPC_THR_HOVER.
 * A value of 0.2 with MPC_THR_HOVER at 0.5 results in a range of [0.3, 0.7].
 *
 * Set to a large value if the vehicle operates in varying physical conditions that
 * affect the required hover thrust strongly (e.g. differently sized payloads).
 *
 * @decimal 2
 * @min 0.01
 * @max 0.4
 * @unit normalized_thrust
 * @group Hover Thrust Estimator
 */
PARAM_DEFINE_FLOAT(HTE_THR_RANGE, 0.2);
