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
 * @file battery_params.c
 *
 * Parameters defined by the battery lib, shared between all batteries.
 *
 * @author Julian Oes <julian@oes.ch>
 */

/**
 * Low threshold
 *
 * Sets the threshold when the battery will be reported as low.
 * This has to be higher than the critical threshold.
 *
 * @group Battery Calibration
 * @unit norm
 * @min 0.12
 * @max 0.5
 * @decimal 2
 * @increment 0.01
 */
PARAM_DEFINE_FLOAT(BAT_LOW_THR, 0.15f);

/**
 * Critical threshold
 *
 * Sets the threshold when the battery will be reported as critically low.
 * This has to be lower than the low threshold. This threshold commonly
 * will trigger RTL.
 *
 * @group Battery Calibration
 * @unit norm
 * @min 0.05
 * @max 0.25
 * @decimal 2
 * @increment 0.01
 */
PARAM_DEFINE_FLOAT(BAT_CRIT_THR, 0.07f);

/**
 * Emergency threshold
 *
 * Sets the threshold when the battery will be reported as dangerously low.
 * This has to be lower than the critical threshold. This threshold commonly
 * will trigger landing.
 *
 * @group Battery Calibration
 * @unit norm
 * @min 0.03
 * @max 0.1
 * @decimal 2
 * @increment 0.01
 */
PARAM_DEFINE_FLOAT(BAT_EMERGEN_THR, 0.05f);

/**
 * Expected battery current in flight.
 *
 * This value is used to initialize the in-flight average current estimation,
 * which in turn is used for estimating remaining flight time and RTL triggering.
 *
 * @group Battery Calibration
 * @unit A
 * @min 0
 * @max 500
 * @increment 0.1
 */
PARAM_DEFINE_FLOAT(BAT_AVRG_CURRENT, 15.0f);
