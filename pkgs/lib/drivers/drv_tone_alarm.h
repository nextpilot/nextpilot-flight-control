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
 * @file drv_tone_alarm.h
 *
 * Driver for the PX4 audio alarm port.
 *
 */

#pragma once

#include "drv_hrt.h"
#include <tunes/tune_definition.h>
#include <uORB/topics/tune_control.h>

namespace ToneAlarmInterface
{

/**
 * @brief Activates/configures the hardware registers.
 */
void init();

/**
 * @brief Starts playing the note.
 * @return the time the note started played.
 */
hrt_abstime start_note(unsigned frequency);

/**
 * @brief Stops playing the current note and makes the player 'safe'.
 */
void stop_note();

} // ToneAlarmInterface
