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
 * @file camera_trigger_params.c
 * Camera trigger parameters
 *
 * @author Mohammed Kabir <kabir@uasys.io>
 * @author Andreas Bircher <andreas@wingtra.com>
 * @author Lorenz Meier <lorenz@px4.io>
 */

/**
* Camera trigger Interface
*
* Selects the trigger interface
*
* @value 1 GPIO
* @value 2 Seagull MAP2 (over PWM)
* @value 3 MAVLink (forward via MAV_CMD_IMAGE_START_CAPTURE)
* @value 4 Generic PWM (IR trigger, servo)
*
* @reboot_required true
* @group Camera trigger
*/
PARAM_DEFINE_INT32(TRIG_INTERFACE, 4);

/**
 * Camera trigger interval
 *
 * This parameter sets the time between two consecutive trigger events
 *
 * @unit ms
 * @min 4.0
 * @max 10000.0
 * @decimal 1
 * @reboot_required true
 * @group Camera trigger
 */
PARAM_DEFINE_FLOAT(TRIG_INTERVAL, 40.0f);

/**
 * Minimum camera trigger interval
 *
 * This parameter sets the minimum time between two consecutive trigger events
 * the specific camera setup is supporting.
 *
 * @unit ms
 * @min 1.0
 * @max 10000.0
 * @decimal 1
 * @reboot_required true
 * @group Camera trigger
 */
PARAM_DEFINE_FLOAT(TRIG_MIN_INTERVA, 1.0f);

/**
 * Camera trigger polarity
 *
 * This parameter sets the polarity of the trigger (0 = active low, 1 = active high )
 *
 * @value 0 Active low
 * @value 1 Active high
 * @min 0
 * @max 1
 * @reboot_required true
 * @group Camera trigger
 */
PARAM_DEFINE_INT32(TRIG_POLARITY, 0);

/**
 * Camera trigger activation time
 *
 * This parameter sets the time the trigger needs to pulled high or low.
 *
 * @unit ms
 * @min 0.1
 * @max 3000
 * @decimal 1
 * @reboot_required true
 * @group Camera trigger
 */
PARAM_DEFINE_FLOAT(TRIG_ACT_TIME, 40.0f);

/**
 * Camera trigger mode
 *
 * @value 0 Disable
 * @value 1 Time based, on command
 * @value 2 Time based, always on
 * @value 3 Distance based, always on
 * @value 4 Distance based, on command (Survey mode)
 * @min 0
 * @max 4
 * @reboot_required true
 * @group Camera trigger
 */
PARAM_DEFINE_INT32(TRIG_MODE, 0);

/**
 * Camera trigger distance
 *
 * Sets the distance at which to trigger the camera.
 *
 * @unit m
 * @min 0
 * @increment 1
 * @decimal 1
 * @reboot_required true
 * @group Camera trigger
 */
PARAM_DEFINE_FLOAT(TRIG_DISTANCE, 25.0f);

/**
 * PWM output to trigger shot.
 *
 * @min 1000
 * @max 2000
 * @unit us
 * @group Camera trigger
 * @reboot_required true
 */
PARAM_DEFINE_INT32(TRIG_PWM_SHOOT, 1900);


/**
 * PWM neutral output on trigger pin.
 *
 * @min 1000
 * @max 2000
 * @unit us
 * @group Camera trigger
 * @reboot_required true
 */
PARAM_DEFINE_INT32(TRIG_PWM_NEUTRAL, 1500);
