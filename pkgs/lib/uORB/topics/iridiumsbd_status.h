
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file IridiumsbdStatus.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT iridiumsbd_status_s {
#else
struct iridiumsbd_status_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint64_t last_heartbeat;
	uint16_t tx_buf_write_index;
	uint16_t rx_buf_read_index;
	uint16_t rx_buf_end_index;
	uint16_t failed_sbd_sessions;
	uint16_t successful_sbd_sessions;
	uint16_t num_tx_buf_reset;
	uint8_t signal_quality;
	uint8_t state;
	bool ring_pending;
	bool tx_buf_write_pending;
	bool tx_session_pending;
	bool rx_read_pending;
	bool rx_session_pending;
	uint8_t _padding0[5]; // required for logger

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using IridiumsbdStatus = iridiumsbd_status_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(iridiumsbd_status);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const iridiumsbd_status_s& message);
#endif // __cplusplus
