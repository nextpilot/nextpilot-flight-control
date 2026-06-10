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
 * @file DShot servo output interface.
 *
 */

#pragma once

#include <defines.h>

#include <stdint.h>
#include <sys/ioctl.h>
#include <board_config.h>

#include "drv_orb_dev.h"

__BEGIN_DECLS

typedef enum {
	DShot_cmd_motor_stop = 0,
	DShot_cmd_beacon1,
	DShot_cmd_beacon2,
	DShot_cmd_beacon3,
	DShot_cmd_beacon4,
	DShot_cmd_beacon5,
	DShot_cmd_esc_info, // V2 includes settings
	DShot_cmd_spin_direction_1,
	DShot_cmd_spin_direction_2,
	DShot_cmd_3d_mode_off,
	DShot_cmd_3d_mode_on,
	DShot_cmd_settings_request, // Currently not implemented
	DShot_cmd_save_settings,
	DShot_cmd_spin_direction_normal   = 20,
	DShot_cmd_spin_direction_reversed = 21,
	DShot_cmd_led0_on,      // BLHeli32 only
	DShot_cmd_led1_on,      // BLHeli32 only
	DShot_cmd_led2_on,      // BLHeli32 only
	DShot_cmd_led3_on,      // BLHeli32 only
	DShot_cmd_led0_off,     // BLHeli32 only
	DShot_cmd_led1_off,     // BLHeli32 only
	DShot_cmd_led2_off,     // BLHeli32 only
	DShot_cmd_led4_off,     // BLHeli32 only
	DShot_cmd_audio_stream_mode_on_off              = 30, // KISS audio Stream mode on/off
	DShot_cmd_silent_mode_on_off                    = 31, // KISS silent Mode on/off
	DShot_cmd_signal_line_telemetry_disable         = 32,
	DShot_cmd_signal_line_continuous_erpm_telemetry = 33,
	DShot_cmd_MAX          = 47,     // >47 are throttle values
	DShot_cmd_MIN_throttle = 48,
	DShot_cmd_MAX_throttle = 2047
} dshot_command_t;


/**
 * Intialise the Dshot outputs using the specified configuration.
 *
 * @param channel_mask		Bitmask of channels (LSB = channel 0) to enable.
 *				This allows some of the channels to remain configured
 *				as GPIOs or as another function. Already used channels/timers will not be configured as DShot
 * @param dshot_pwm_freq	Frequency of DSHOT signal. Usually DSHOT150, DSHOT300, DSHOT600 or DSHOT1200
 * @return <0 on error, the initialized channels mask.
 */
__EXPORT extern int up_dshot_init(uint32_t channel_mask, unsigned dshot_pwm_freq);

/**
 * Set Dshot motor data, used by up_dshot_motor_data_set() and up_dshot_motor_command() (internal method)
 */
__EXPORT extern void dshot_motor_data_set(unsigned motor_number, uint16_t throttle, bool telemetry);

/**
 * Set the current dshot throttle value for a channel (motor).
 *
 * @param channel	The channel to set.
 * @param throttle	The output dshot throttle value in [0 = DSHOT_DISARM_VALUE, 1 = DSHOT_MIN_THROTTLE, 1999 = DSHOT_MAX_THROTTLE].
 * @param telemetry	If true, request telemetry from that motor
 */
static inline void up_dshot_motor_data_set(unsigned channel, uint16_t throttle, bool telemetry)
{
	dshot_motor_data_set(channel, throttle + DShot_cmd_MIN_throttle, telemetry);
}

/**
 * Send DShot command to a channel (motor).
 *
 * @param channel	The channel to set.
 * @param command	dshot_command_t
 * @param telemetry	If true, request telemetry from that motor
 */
static inline void up_dshot_motor_command(unsigned channel, uint16_t command, bool telemetry)
{
	dshot_motor_data_set(channel, command, telemetry);
}

/**
 * Trigger dshot data transfer.
 */
__EXPORT extern void up_dshot_trigger(void);

/**
 * Arm or disarm dshot outputs (This will enable/disable complete timer for safety purpose.).
 *
 * When disarmed, dshot output no pulse.
 *
 * @param armed		If true, outputs are armed; if false they
 *			are disarmed.
 */
__EXPORT extern int up_dshot_arm(bool armed);

__END_DECLS
