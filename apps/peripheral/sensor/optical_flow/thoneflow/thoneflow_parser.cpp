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
 * @author Mohammed Kabir <mhkabir98@gmail.com>
 *
 * Declarations of parser for the ThoneFlow-3901U optical flow sensor
 */

#include "thoneflow_parser.h"
#include <string.h>
#include <stdlib.h>

//#define THONEFLOW_DEBUG

#ifdef THONEFLOW_DEBUG
#   include <stdio.h>

const char *parser_state[] = {
    "0_UNSYNC",
    "1_HEADER",
    "2_NBYTES",
    "3_XM_L",
    "4_XM_H",
    "5_YM_L",
    "6_YM_H",
    "7_CHECKSUM",
    "8_QUALITY",
    "9_FOOTER"};
#endif

bool thoneflow_parse(char c, char *parserbuf, unsigned *parserbuf_index, enum THONEFLOW_PARSE_STATE *state,
                     sensor_optical_flow_s *flow) {
    bool parsed_packet = false;

    switch (*state) {
    case THONEFLOW_PARSE_STATE9_FOOTER:
        if (c == 0xFE) {
            *state = THONEFLOW_PARSE_STATE1_HEADER;

        } else {
            *state = THONEFLOW_PARSE_STATE0_UNSYNC;
        }

        break;

    case THONEFLOW_PARSE_STATE0_UNSYNC:
        if (c == 0xFE) {
            *state = THONEFLOW_PARSE_STATE1_HEADER;
        }

        break;

    case THONEFLOW_PARSE_STATE1_HEADER:
        if (c == 0x04) {
            *state = THONEFLOW_PARSE_STATE2_NBYTES;

        } else {
            *state = THONEFLOW_PARSE_STATE0_UNSYNC;
        }

        break;

    case THONEFLOW_PARSE_STATE2_NBYTES:
        *state                      = THONEFLOW_PARSE_STATE3_XM_L;
        parserbuf[*parserbuf_index] = c;
        (*parserbuf_index)++;

        break;

    case THONEFLOW_PARSE_STATE3_XM_L:
        *state                      = THONEFLOW_PARSE_STATE4_XM_H;
        parserbuf[*parserbuf_index] = c;
        (*parserbuf_index)++;

        break;

    case THONEFLOW_PARSE_STATE4_XM_H:
        *state                      = THONEFLOW_PARSE_STATE5_YM_L;
        parserbuf[*parserbuf_index] = c;
        (*parserbuf_index)++;

        break;

    case THONEFLOW_PARSE_STATE5_YM_L:
        *state                      = THONEFLOW_PARSE_STATE6_YM_H;
        parserbuf[*parserbuf_index] = c;
        (*parserbuf_index)++;

        break;

    case THONEFLOW_PARSE_STATE6_YM_H: {
        unsigned char cksm = 0;

        // Calculate checksum over motion values
        for (int i = 0; i < 4; i++) {
            cksm += parserbuf[i];
        }

        if (c == cksm) {
            // Checksum valid, populate sensor report
            int16_t delta_x     = uint16_t(parserbuf[1]) << 8 | parserbuf[0];
            int16_t delta_y     = uint16_t(parserbuf[3]) << 8 | parserbuf[2];
            flow->pixel_flow[0] = static_cast<float>(delta_x) * (3.52e-3f);
            flow->pixel_flow[1] = static_cast<float>(delta_y) * (3.52e-3f);
            *state              = THONEFLOW_PARSE_STATE7_CHECKSUM;

        } else {
            *state = THONEFLOW_PARSE_STATE0_UNSYNC;
        }

        *parserbuf_index = 0;
    }

    break;

    case THONEFLOW_PARSE_STATE7_CHECKSUM:
        *state        = THONEFLOW_PARSE_STATE8_QUALITY;
        flow->quality = uint8_t(c);

        break;

    case THONEFLOW_PARSE_STATE8_QUALITY:
        if (c == 0xAA) {
            *state        = THONEFLOW_PARSE_STATE9_FOOTER;
            parsed_packet = true;

        } else {
            *state = THONEFLOW_PARSE_STATE0_UNSYNC;
        }

        break;
    }

#ifdef THONEFLOW_DEBUG
    printf("state: THONEFLOW_PARSE_STATE%s, got char: %#02x\n", parser_state[*state], c);
#endif

    return parsed_packet;
}
