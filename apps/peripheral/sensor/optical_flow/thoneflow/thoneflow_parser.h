/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <uORB/topics/sensor_optical_flow.h>

// Data Format for ThoneFlow 3901U
// ===============================
// 9 bytes total per message:
// 1) Header (0xFE)
// 2) Number of data bytes (0x04)
// 3) X motion low byte
// 4) X motion high byte
// 5) Y motion low byte
// 6) Y motion high byte
// 7) Checksum (byte3+...+byte6)
// 8) Quality
// 9) Footer (0xAA)

enum THONEFLOW_PARSE_STATE {
    THONEFLOW_PARSE_STATE0_UNSYNC = 0,
    THONEFLOW_PARSE_STATE1_HEADER,
    THONEFLOW_PARSE_STATE2_NBYTES,
    THONEFLOW_PARSE_STATE3_XM_L,
    THONEFLOW_PARSE_STATE4_XM_H,
    THONEFLOW_PARSE_STATE5_YM_L,
    THONEFLOW_PARSE_STATE6_YM_H,
    THONEFLOW_PARSE_STATE7_CHECKSUM,
    THONEFLOW_PARSE_STATE8_QUALITY,
    THONEFLOW_PARSE_STATE9_FOOTER
};

bool thoneflow_parse(char c, char *parserbuf, unsigned *parserbuf_index, enum THONEFLOW_PARSE_STATE *state,
                     sensor_optical_flow_s *report);
