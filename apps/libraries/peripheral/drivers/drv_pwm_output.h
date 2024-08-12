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
 * @file PWM servo output interface.
 *
 */

#pragma once

#include <defines.h>

#include <stdint.h>
// #include <sys/ioctl.h>
// #include <board.h>

#include "drv_orb_dev.h"

__BEGIN_DECLS

#define PWM_OUTPUT_MAX_CHANNELS 16

/* Use defaults unless the board override the defaults by providing
 * PX4_PWM_ALTERNATE_RANGES and a replacement set of
 * constants
 */
#if !defined(PX4_PWM_ALTERNATE_RANGES)

/**
 * Lowest minimum PWM in us
 */
#define PWM_LOWEST_MIN 90

/**
 * Highest maximum PWM in us
 */
#define PWM_HIGHEST_MAX 2500

#endif // not PX4_PWM_ALTERNATE_RANGES

/**
 * Do not output a channel with this value
 */
#define PWM_IGNORE_THIS_CHANNEL UINT16_MAX

/*
 * ioctl() definitions
 *
 * Note that ioctls and ORB updates should not be mixed, as the
 * behaviour of the system in this case is not defined.
 */
#define _PWM_SERVO_BASE		0x2a00

/** start DSM bind */
#define DSM_BIND_START	_PX4_IOC(_PWM_SERVO_BASE, 10)

/** specific rates for configuring the timer for OneShot or PWM */
#define	PWM_RATE_ONESHOT			0u
#define	PWM_RATE_LOWER_LIMIT		1u
#define	PWM_RATE_UPPER_LIMIT		10000u

/*
 * Low-level PWM output interface.
 *
 * This is the low-level API to the platform-specific PWM driver.
 */

/**
 * Intialise the PWM servo outputs using the specified configuration.
 *
 * @param channel_mask	Bitmask of channels (LSB = channel 0) to enable.
 *			This allows some of the channels to remain configured
 *			as GPIOs or as another function. Already used channels/timers
 *			will not be configured as PWM.
 * @return <0 on error, the initialized channels mask.
 */
__EXPORT extern int	up_pwm_servo_init(uint32_t channel_mask);

/**
 * De-initialise the PWM servo outputs.
 *
 * @param channel_mask  Bitmask of channels (LSB = channel 0) to enable.
 *      This allows some of the channels to remain configured
 *      as GPIOs or as another function.
 *      A value of 0 is ALL channels
 *
 */
__EXPORT extern void	up_pwm_servo_deinit(uint32_t channel_mask);

/**
 * Arm or disarm servo outputs.
 *
 * When disarmed, servos output no pulse.
 *
 * @bug This function should, but does not, guarantee that any pulse
 *      currently in progress is cleanly completed.
 *
 * @param armed		If true, outputs are armed; if false they
 *			are disarmed.
 *
 * @param channel_mask  Bitmask of channels (LSB = channel 0) to enable.
 *      This allows some of the channels to remain configured
 *      as GPIOs or as another function.
 *      A value of 0 is ALL channels
 *
 */
__EXPORT extern void	up_pwm_servo_arm(bool armed, uint32_t channel_mask);

/**
 * Set the servo update rate for all rate groups.
 *
 * @param rate		The update rate in Hz to set.
 * @return		OK on success, -ERANGE if an unsupported update rate is set.
 */
__EXPORT extern int	up_pwm_servo_set_rate(unsigned rate);

/**
 * Get a bitmap of output channels assigned to a given rate group.
 *
 * @param group		The rate group to query. Rate groups are assigned contiguously
 *			starting from zero.
 * @return		A bitmap of channels assigned to the rate group, or zero if
 *			the group number has no channels.
 */
__EXPORT extern uint32_t up_pwm_servo_get_rate_group(unsigned group);

/**
 * Set the update rate for a given rate group.
 *
 * @param group		The rate group whose update rate will be changed.
 * @param rate		The update rate in Hz.
 * @return		OK if the group was adjusted, -ERANGE if an unsupported update rate is set.
 */
__EXPORT extern int	up_pwm_servo_set_rate_group_update(unsigned group, unsigned rate);

/**
 * Trigger all timer's channels in Oneshot mode to fire
 * the oneshot with updated values.
 * Nothing is done if not in oneshot mode.
 *
 */
__EXPORT extern void up_pwm_update(unsigned channel_mask);

/**
 * Set the current output value for a channel.
 *
 * @param channel	The channel to set.
 * @param value		The output pulse width in microseconds.
 */
__EXPORT extern int	up_pwm_servo_set(unsigned channel, uint16_t value);

/**
 * Get the current output value for a channel.
 *
 * @param channel	The channel to read.
 * @return		The output pulse width in microseconds, or zero if
 *			outputs are not armed or not configured.
 */
__EXPORT extern uint16_t up_pwm_servo_get(unsigned channel);


__END_DECLS
