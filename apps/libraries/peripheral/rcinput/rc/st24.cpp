/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/*
 * @file st24.h
 *
 * RC protocol implementation for Yuneec ST24 transmitter.
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 */

#include <stdbool.h>
#include <stdio.h>
#include "st24.h"
#include "common_rc.h"

const char *decode_states[] = {"UNSYNCED",
			       "GOT_STX1",
			       "GOT_STX2",
			       "GOT_LEN",
			       "GOT_TYPE",
			       "GOT_DATA"
			      };

/* define range mapping here, -+100% -> 1000..2000 */
#define ST24_RANGE_MIN 500.0f
#define ST24_RANGE_MAX 3500.0f

#define ST24_TARGET_MIN 1000.0f
#define ST24_TARGET_MAX 2000.0f

/* pre-calculate the floating point stuff as far as possible at compile time */
#define ST24_SCALE_FACTOR ((ST24_TARGET_MAX - ST24_TARGET_MIN) / (ST24_RANGE_MAX - ST24_RANGE_MIN))
#define ST24_SCALE_OFFSET (int)(ST24_TARGET_MIN - (ST24_SCALE_FACTOR * ST24_RANGE_MIN + 0.5f))

static enum ST24_DECODE_STATE _decode_state = ST24_DECODE_STATE_UNSYNCED;
static uint8_t _rxlen;

static ReceiverFcPacket &_rxpacket = rc_decode_buf._strxpacket;

uint8_t st24_common_crc8(uint8_t *ptr, uint8_t len)
{
	uint8_t i, crc ;
	crc = 0;

	while (len--) {
		for (i = 0x80; i != 0; i >>= 1) {
			if ((crc & 0x80) != 0) {
				crc <<= 1;
				crc ^= 0x07;

			} else {
				crc <<= 1;
			}

			if ((*ptr & i) != 0) {
				crc ^= 0x07;
			}
		}

		ptr++;
	}

	return (crc);
}


int st24_decode(uint8_t byte, uint8_t *rssi, uint8_t *lost_count, uint16_t *channel_count, uint16_t *channels,
		uint16_t max_chan_count)
{
	int ret = 1;

	switch (_decode_state) {
	case ST24_DECODE_STATE_UNSYNCED:
		if (byte == ST24_STX1) {
			_decode_state = ST24_DECODE_STATE_GOT_STX1;

		} else {
			ret = 3;
		}

		break;

	case ST24_DECODE_STATE_GOT_STX1:
		if (byte == ST24_STX2) {
			_decode_state = ST24_DECODE_STATE_GOT_STX2;

		} else {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		break;

	case ST24_DECODE_STATE_GOT_STX2:

		/* ensure no data overflow failure or hack is possible */
		if ((unsigned)byte <= sizeof(_rxpacket.length) + sizeof(_rxpacket.type) + sizeof(_rxpacket.st24_data)) {
			_rxpacket.length = byte;
			_rxlen = 0;
			_decode_state = ST24_DECODE_STATE_GOT_LEN;

		} else {
			_decode_state = ST24_DECODE_STATE_UNSYNCED;
		}

		break;

	case ST24_DECODE_STATE_GOT_LEN:
		_rxpacket.type = byte;
		_rxlen++;
		_decode_state = ST24_DECODE_STATE_GOT_TYPE;
		break;

	case ST24_DECODE_STATE_GOT_TYPE:
		_rxpacket.st24_data[_rxlen - 1] = byte;
		_rxlen++;

		if (_rxlen == (_rxpacket.length - 1)) {
			_decode_state = ST24_DECODE_STATE_GOT_DATA;
		}

		break;

	case ST24_DECODE_STATE_GOT_DATA:
		_rxpacket.crc8 = byte;
		_rxlen++;

		if (st24_common_crc8((uint8_t *) & (_rxpacket.length), _rxlen) == _rxpacket.crc8) {

			ret = 0;

			/* decode the actual packet */

			switch (_rxpacket.type) {

			case ST24_PACKET_TYPE_CHANNELDATA12: {
					ChannelData12 *d = (ChannelData12 *)_rxpacket.st24_data;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*lost_count = d->lost_count;

					/* this can lead to rounding of the strides */
					*channel_count = (max_chan_count < 12) ? max_chan_count : 12;

					unsigned stride_count = (*channel_count * 3) / 2;
					unsigned chan_index = 0;

					for (unsigned i = 0; i < stride_count; i += 3) {
						channels[chan_index] = ((uint16_t)d->channel[i] << 4);
						channels[chan_index] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = (uint16_t)(channels[chan_index] * ST24_SCALE_FACTOR + .5f) + ST24_SCALE_OFFSET;
						chan_index++;

						channels[chan_index] = ((uint16_t)d->channel[i + 2]);
						channels[chan_index] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = (uint16_t)(channels[chan_index] * ST24_SCALE_FACTOR + .5f) + ST24_SCALE_OFFSET;
						chan_index++;
					}
				}
				break;

			case ST24_PACKET_TYPE_CHANNELDATA24: {
					ChannelData24 *d = (ChannelData24 *)&_rxpacket.st24_data;

					// Scale from 0..255 to 100%.
					*rssi = d->rssi * (100.0f / 255.0f);
					*lost_count = d->lost_count;

					/* this can lead to rounding of the strides */
					*channel_count = (max_chan_count < 24) ? max_chan_count : 24;

					unsigned stride_count = (*channel_count * 3) / 2;
					unsigned chan_index = 0;

					for (unsigned i = 0; i < stride_count; i += 3) {
						channels[chan_index] = ((uint16_t)d->channel[i] << 4);
						channels[chan_index] |= ((uint16_t)(0xF0 & d->channel[i + 1]) >> 4);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = (uint16_t)(channels[chan_index] * ST24_SCALE_FACTOR + .5f) + ST24_SCALE_OFFSET;
						chan_index++;

						channels[chan_index] = ((uint16_t)d->channel[i + 2]);
						channels[chan_index] |= (((uint16_t)(0x0F & d->channel[i + 1])) << 8);
						/* convert values to 1000-2000 ppm encoding in a not too sloppy fashion */
						channels[chan_index] = (uint16_t)(channels[chan_index] * ST24_SCALE_FACTOR + .5f) + ST24_SCALE_OFFSET;
						chan_index++;
					}
				}
				break;

			case ST24_PACKET_TYPE_TRANSMITTERGPSDATA: {

					// ReceiverFcPacket* d = (ReceiverFcPacket*)&_rxpacket.st24_data;
					/* we silently ignore this data for now, as it is unused */
					ret = 5;
				}
				break;

			default:
				ret = 2;
				break;
			}

		} else {
			/* decoding failed */
			ret = 4;
		}

		_decode_state = ST24_DECODE_STATE_UNSYNCED;
		break;
	}

	return ret;
}
