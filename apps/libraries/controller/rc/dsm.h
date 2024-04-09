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
 * @file dsm.h
 *
 * RC protocol definition for Spektrum RC
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#pragma once

#include <stdint.h>

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>

__BEGIN_DECLS

#define DSM_FRAME_SIZE		16		/**< DSM frame size in bytes */
#define DSM_FRAME_CHANNELS	7		/**< Max supported DSM channels per frame */
#define DSM_MAX_CHANNEL_COUNT   20  /**< Max channel count of any DSM RC */
#define DSM_BUFFER_SIZE		(DSM_FRAME_SIZE + DSM_FRAME_SIZE / 2)


#pragma pack(push, 1)
typedef   uint8_t dsm_frame_t[DSM_BUFFER_SIZE]; /**< DSM dsm frame receive buffer */
typedef   uint8_t dsm_buf_t[DSM_FRAME_SIZE * 2]; // Define working buffer

typedef  struct dsm_decode_t {
	dsm_frame_t frame;
	dsm_buf_t buf;
} dsm_decode_t;

#pragma pack(pop)

__EXPORT int	dsm_init(const char *device);
__EXPORT void	dsm_deinit(void);
__EXPORT void	dsm_proto_init(void);
__EXPORT int	dsm_config(int dsm_fd);
__EXPORT bool	dsm_input(int dsm_fd, uint16_t *values, uint16_t *num_values, bool *dsm_11_bit, uint8_t *n_bytes,
			  uint8_t **bytes, int8_t *rssi, unsigned *frame_drops, unsigned max_values);

__EXPORT bool	dsm_parse(const uint64_t now, const uint8_t *frame, const unsigned len, uint16_t *values,
			  uint16_t *num_values, bool *dsm_11_bit, unsigned *frame_drops, int8_t *rssi_percent, uint16_t max_channels);

#ifdef SPEKTRUM_POWER
__EXPORT void	dsm_bind(uint16_t cmd, int pulses);
#endif

enum DSM_CMD {							/* DSM bind states */
	DSM_CMD_BIND_POWER_DOWN = 0,
	DSM_CMD_BIND_POWER_UP,
	DSM_CMD_BIND_SET_RX_OUT,
	DSM_CMD_BIND_SEND_PULSES,
	DSM_CMD_BIND_REINIT_UART
};

#define DSM2_BIND_PULSES 3	/* DSM_BIND_START parameter, pulses required to start pairing DSM2 22ms */
#define DSMX_BIND_PULSES 7	/* DSM_BIND_START parameter, pulses required to start pairing DSMx 22ms */
#define DSMX8_BIND_PULSES 9     /* DSM_BIND_START parameter, pulses required to start pairing DSMx 11ms */

__END_DECLS
