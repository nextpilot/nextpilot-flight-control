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
 * @file crsf.h
 *
 * RC protocol definition for CSRF (TBS Crossfire).
 * It is an uninverted protocol at 420000 baudrate.
 *
 * RC channels come in at 150Hz.
 *
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#pragma once

#include <stdint.h>
#include <px4_platform_common/defines.h>

__BEGIN_DECLS

#define CRSF_FRAME_SIZE_MAX 30 // the actual maximum length is 64, but we're only interested in RC channels and want to minimize buffer size
#define CRSF_PAYLOAD_SIZE_MAX (CRSF_FRAME_SIZE_MAX-4)


struct crsf_frame_header_t {
	uint8_t device_address;             ///< @see crsf_address_t
	uint8_t length;                     ///< length of crsf_frame_t (including CRC) minus sizeof(crsf_frame_header_t)
};

struct crsf_frame_t {
	crsf_frame_header_t header;
	uint8_t type;                       ///< @see crsf_frame_type_t
	uint8_t payload[CRSF_PAYLOAD_SIZE_MAX + 1]; ///< payload data including 1 byte CRC at end
};

/**
 * Configure an UART port to be used for CRSF
 * @param uart_fd UART file descriptor
 * @return 0 on success, -errno otherwise
 */
__EXPORT int	crsf_config(int uart_fd);


/**
 * Parse the CRSF protocol and extract RC channel data.
 *
 * @param now current time
 * @param frame data to parse
 * @param len length of frame
 * @param values output channel values, each in range [1000, 2000]
 * @param num_values set to the number of parsed channels in values
 * @param max_channels maximum length of values
 * @return true if channels successfully decoded
 */
__EXPORT bool	crsf_parse(const uint64_t now, const uint8_t *frame, unsigned len, uint16_t *values,
			   uint16_t *num_values, uint16_t max_channels);


/**
 * Send telemetry battery information
 * @param uart_fd UART file descriptor
 * @param voltage Voltage [0.1V]
 * @param current Current [0.1A]
 * @param fuel drawn mAh
 * @param remaining battery remaining [%]
 * @return true on success
 */
__EXPORT bool crsf_send_telemetry_battery(int uart_fd, uint16_t voltage, uint16_t current, int fuel, uint8_t remaining);

/**
 * Send telemetry GPS information
 * @param uart_fd UART file descriptor
 * @param latitude latitude [degree * 1e7]
 * @param longitude longitude [degree * 1e7]
 * @param groundspeed Ground speed [km/h * 10]
 * @param gps_heading GPS heading [degree * 100]
 * @param altitude Altitude [meters + 1000m offset]
 * @param num_satellites number of satellites used
 * @return true on success
 */
__EXPORT bool crsf_send_telemetry_gps(int uart_fd, int32_t latitude, int32_t longitude, uint16_t groundspeed,
				      uint16_t gps_heading, uint16_t altitude, uint8_t num_satellites);


/**
 * Send telemetry Attitude information
 * @param uart_fd UART file descriptor
 * @param pitch Pitch angle [rad * 1e4]
 * @param roll Roll angle [rad * 1e4]
 * @param yaw Yaw angle [rad * 1e4]
 * @return true on success
 */
__EXPORT bool crsf_send_telemetry_attitude(int uart_fd, int16_t pitch, int16_t roll, int16_t yaw);

/**
 * Send telemetry Flight Mode information
 * @param uart_fd UART file descriptor
 * @param flight_mode Flight Mode string (max length = 15)
 * @return true on success
 */
__EXPORT bool crsf_send_telemetry_flight_mode(int uart_fd, const char *flight_mode);

__END_DECLS
