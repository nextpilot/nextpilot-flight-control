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
 * @file camera_capture_params.c
 * Camera capture parameters
 *
 * @author Mohammed Kabir <kabir@uasys.io>
 */
/**
 * Camera strobe delay
 *
 * This parameter sets the delay between image integration start and strobe firing
 *
 * @unit ms
 * @min 0.0
 * @max 100.0
 * @decimal 1
 * @group Camera Capture
 */
PARAM_DEFINE_FLOAT(CAM_CAP_DELAY, 0.0f);

/**
 * Camera capture feedback
 *
 * Enables camera capture feedback
 *
 * @boolean
 * @group Camera Control
 * @reboot_required true
 */
PARAM_DEFINE_INT32(CAM_CAP_FBACK, 0);

/**
 * Camera capture timestamping mode
 *
 * Change time measurement
 *
 * @value 0 Get absolute timestamp
 * @value 1 Get timestamp of mid exposure (active high)
 * @value 2 Get timestamp of mid exposure (active low)
 *
 * @group Camera Control
 * @reboot_required true
 */
PARAM_DEFINE_INT32(CAM_CAP_MODE, 0);

/**
 * Camera capture edge
 *
 * @value 0 Falling edge
 * @value 1 Rising edge
 *
 * @group Camera Control
 * @reboot_required true
 */
PARAM_DEFINE_INT32(CAM_CAP_EDGE, 0);
