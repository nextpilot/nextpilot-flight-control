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
 * @file sbus.h
 *
 * RC protocol definition for S.BUS
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

#define SBUS_FRAME_SIZE			25
#define SBUS_BUFFER_SIZE		(SBUS_FRAME_SIZE + SBUS_FRAME_SIZE / 2)

typedef   uint8_t sbus_frame_t[SBUS_FRAME_SIZE + (SBUS_FRAME_SIZE / 2)];


__EXPORT int	sbus_init(const char *device, bool singlewire);

/**
 * Parse serial bytes on the S.BUS bus
 *
 * The S.bus protocol doesn't provide reliable framing,
 * so we detect frame boundaries by the inter-frame delay.
 *
 * The minimum frame spacing is 7ms; with 25 bytes at 100000bps
 * frame transmission time is 3ms.
 *
 * If an interval of more than 4ms (7 ms frame spacing plus margin)
 * passes between calls, the first byte we read will be the first
 * byte of a frame.
 *
 * In the case where byte(s) are dropped from a frame, this also
 * provides a degree of protection. Of course, it would be better
 * if we didn't drop bytes...
 */
__EXPORT int	sbus_config(int sbus_fd, bool singlewire);
__EXPORT bool	sbus_input(int sbus_fd, uint16_t *values, uint16_t *num_values, bool *sbus_failsafe,
			   bool *sbus_frame_drop,
			   uint16_t max_channels);
__EXPORT bool	sbus_parse(uint64_t now, uint8_t *frame, unsigned len, uint16_t *values,
			   uint16_t *num_values, bool *sbus_failsafe, bool *sbus_frame_drop, unsigned *frame_drops, uint16_t max_channels);
__EXPORT void	sbus1_output(int sbus_fd, uint16_t *values, uint16_t num_values);
__EXPORT void	sbus2_output(int sbus_fd, uint16_t *values, uint16_t num_values);
__EXPORT void	sbus1_set_output_rate_hz(uint16_t rate_hz);

/**
 * The number of incomplete frames we encountered
 */
__EXPORT unsigned	sbus_dropped_frames(void);

__END_DECLS
