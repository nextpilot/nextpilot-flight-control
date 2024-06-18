/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


/* MAVLink adapter header */
#ifndef MAVLINK_BRIDGE_HEADER_H
#define MAVLINK_BRIDGE_HEADER_H

#define MAVLINK_NO_CONVERSION_HELPERS

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

/* use efficient approach, see mavlink_helpers.h */
#define MAVLINK_SEND_UART_BYTES    mavlink_send_uart_bytes

#define MAVLINK_START_UART_SEND    mavlink_start_uart_send
#define MAVLINK_END_UART_SEND      mavlink_end_uart_send

#define MAVLINK_GET_CHANNEL_BUFFER mavlink_get_channel_buffer
#define MAVLINK_GET_CHANNEL_STATUS mavlink_get_channel_status

#if !defined(CONSTRAINED_MEMORY)
#   define MAVLINK_COMM_NUM_BUFFERS 6
#   define MAVLINK_COMM_4           static_cast<mavlink_channel_t>(4)
#   define MAVLINK_COMM_5           static_cast<mavlink_channel_t>(5)
#endif

#include <mavlink_types.h>
#include <unistd.h>

__BEGIN_DECLS

/* Struct that stores the communication settings of this system.
   you can also define / alter these settings elsewhere, as long
   as they're included BEFORE mavlink.h.
   So you can set the

   mavlink_system.sysid = 100; // System ID, 1-255
   mavlink_system.compid = 50; // Component/Subsystem ID, 1-255

   Lines also in your main.c, e.g. by reading these parameter from EEPROM.
 */
extern mavlink_system_t mavlink_system;

/**
 * @brief Send multiple chars (uint8_t) over a comm channel
 *
 * @param chan MAVLink channel to use, usually MAVLINK_COMM_0 = UART0
 * @param ch Character to send
 */
void mavlink_send_uart_bytes(mavlink_channel_t chan, const uint8_t *ch, int length);

void mavlink_start_uart_send(mavlink_channel_t chan, int length);
void mavlink_end_uart_send(mavlink_channel_t chan, int length);

extern mavlink_status_t  *mavlink_get_channel_status(uint8_t chan);
extern mavlink_message_t *mavlink_get_channel_buffer(uint8_t chan);

#include <mavlink.h>
// #if !MAVLINK_FTP_UNIT_TEST
// #   include <uAvionix.h>
// #endif

__END_DECLS

#endif /* MAVLINK_BRIDGE_HEADER_H */
