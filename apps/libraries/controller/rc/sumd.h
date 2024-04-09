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
 * @file sumd.h
 *
 * RC protocol definition for Graupner HoTT transmitter
 *
 * @author Marco Bauer <marco@wtns.de>
 */

#pragma once

#include <stdint.h>

__BEGIN_DECLS

#define SUMD_MAX_CHANNELS	32
#define SUMD_HEADER_LENGTH	3
#define SUMD_HEADER_ID		0xA8
#define SUMD_ID_SUMH		0x00
#define SUMD_ID_SUMD		0x01
#define SUMD_ID_FAILSAFE	0x81




#pragma pack(push, 1)
typedef struct {
	uint8_t	header;							///< 0xA8 for a valid packet
	uint8_t	status;							///< 0x01 valid and live SUMD data frame / 0x00 = SUMH / 0x81 = Failsafe
	uint8_t	length;							///< Channels
	uint8_t	sumd_data[SUMD_MAX_CHANNELS * 2];	///< ChannelData (High Byte/ Low Byte)
	uint8_t	crc16_high;						///< High Byte of 16 Bit CRC
	uint8_t	crc16_low;						///< Low Byte of 16 Bit CRC
	uint8_t	telemetry;						///< Telemetry request
	uint8_t	crc8;							///< SUMH CRC8
} ReceiverFcPacketHoTT;
#pragma pack(pop)


/**
 * CRC16 implementation for SUMD protocol
 *
 * @param crc Initial CRC Value
 * @param value to accumulate in the checksum
 * @return the checksum
 */
uint16_t sumd_crc16(uint16_t crc, uint8_t value);

/**
 * CRC8 implementation for SUMH protocol
 *
 * @param crc Initial CRC Value
 * @param value to accumulate in the checksum
 * @return the checksum
 */
uint8_t sumd_crc8(uint8_t crc, uint8_t value);

/**
 * Decoder for SUMD/SUMH protocol
 *
 * @param byte current char to read
 * @param rssi pointer to a byte where the RSSI value is written back to
 * @param rx_count pointer to a byte where the receive count of packets signce last wireless frame is written back to
 * @param channels pointer to a datastructure of size max_chan_count where channel values (12 bit) are written back to
 * @param max_chan_count maximum channels to decode - if more channels are decoded, the last n are skipped and success (0) is returned
 * @param failsafe pointer to a boolean where the decoded failsafe flag is written back to
 * @return 0 for success (a decoded packet), 1 for no packet yet (accumulating), 2 for unknown packet, 3 for out of sync, 4 for checksum error
 */
/*
__EXPORT int sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_count, uint16_t *channel_count,
				 uint16_t *channels, uint16_t max_chan_count, bool *failsafe);
*/
__EXPORT int sumd_decode(uint8_t byte, uint8_t *rssi, uint8_t *rx_count, uint16_t *channel_count,
			 uint16_t *channels, uint16_t max_chan_count, bool *failsafe);


__END_DECLS
