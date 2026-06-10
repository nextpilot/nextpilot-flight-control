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
 * @file PWM trigger output interface.
 *
 */

#pragma once

#include <defines.h>

#include <stdint.h>

__BEGIN_DECLS

/**
 * Intialise the PWM servo outputs using the specified configuration.
 *
 * @param channel_mask	Bitmask of channels (LSB = channel 0) to enable.
 *			This allows some of the channels to remain configured
 *			as GPIOs or as another function. Already used channels/timers
 *			will not be configured as PWM.
 * @return <0 on error, the initialized channels mask.
 */
__EXPORT extern int	up_pwm_trigger_init(uint32_t channel_mask);

/**
 * De-initialise the PWM trigger outputs.
 */
__EXPORT extern void	up_pwm_trigger_deinit(void);

/**
 * Arm or disarm trigger outputs.
 *
 * @bug This function should, but does not, guarantee that any pulse
 *      currently in progress is cleanly completed.
 *
 * @param armed		If true, outputs are armed; if false they
 *			are disarmed.
 */
__EXPORT extern void	up_pwm_trigger_arm(bool armed);

/**
 * Set the current output value for a channel.
 *
 * @param channel	The channel to set.
 * @param value		The output pulse width in microseconds.
 */
__EXPORT extern int	up_pwm_trigger_set(unsigned channel, uint16_t value);

__END_DECLS
