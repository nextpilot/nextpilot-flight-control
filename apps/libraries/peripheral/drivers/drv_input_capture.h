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
 * @file Input capture interface.
 *
 * Input capture interface utilizes the FMU_AUX_PINS to time stamp
 * an edge.
 */

#pragma once

#include <defines.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include "drv_hrt.h"

__BEGIN_DECLS

typedef void (*capture_callback_t)(void *context, uint32_t chan_index,
				   hrt_abstime edge_time, uint32_t edge_state, uint32_t overflow);

typedef uint16_t capture_filter_t;

typedef enum input_capture_edge {
	Disabled 	= 	0,
	Rising 	 	=   1,
	Falling		=   2,
	Both		=   3
} input_capture_edge;

typedef struct input_capture_stats_t {
	uint32_t 		   edges;
	uint32_t 		   overflows;
	uint32_t		   last_edge;
	hrt_abstime		   last_time;
	uint16_t		   latency;
} input_capture_stats_t;


__EXPORT int up_input_capture_set(unsigned channel, input_capture_edge edge, capture_filter_t filter,
				  capture_callback_t callback, void *context);

__EXPORT int up_input_capture_get_filter(unsigned channel, capture_filter_t *filter);
__EXPORT int up_input_capture_set_filter(unsigned channel,  capture_filter_t filter);

__EXPORT int up_input_capture_get_trigger(unsigned channel,  input_capture_edge *edge);
__EXPORT int up_input_capture_set_trigger(unsigned channel,  input_capture_edge edge);
__EXPORT int up_input_capture_get_callback(unsigned channel, capture_callback_t *callback, void **context);
__EXPORT int up_input_capture_set_callback(unsigned channel, capture_callback_t callback, void *context);
__EXPORT int up_input_capture_get_stats(unsigned channel, input_capture_stats_t *stats, bool clear);
__END_DECLS
