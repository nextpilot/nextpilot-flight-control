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
 * @file pps_capture_params.c
 * PPS Capture params
 */
/**
 * PPS Capture Enable
 *
 * Enables the PPS capture module.
 * This switches mode of FMU channel 7 to be the
 * PPS input channel.
 *
 * @boolean
 * @group GPS
 * @reboot_required true
 */
PARAM_DEFINE_INT32(PPS_CAP_ENABLE, 0);
