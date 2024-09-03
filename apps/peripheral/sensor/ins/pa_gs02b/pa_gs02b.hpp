/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef _PA_GS02B_HPP_
#define _PA_GS02B_HPP_

#include "nextpilot.h"

#define PA_GS02B_SYNC1  (0xDA)
#define PA_GS02B_SYNC2  (0x61)
#define PA_GS02B_LENGTH (78)

#pragma pack(push, 1)

typedef struct {
    float  gyro_deg_s[3];
    float  accel_g[3];
    float  mag_uT[3];
    float  baro_mbar;
    float  roll_deg;
    float  pitch_deg;
    float  yaw_deg;
    float  vel_east_m_s;
    float  vel_north_m_s;
    float  vel_up_m_s;
    double lat_deg;
    double lon_deg;
    float  alt_m;

    union {
        uint8_t flag;

        struct {
            uint8_t output_freq            : 2;
            uint8_t mag_valid              : 1;
            uint8_t baro_valid             : 1;
            uint8_t bestxyz_satellite_used : 4;
        };
    };

    uint8_t pos_type;
    uint8_t vel_type;
    uint8_t hdg_type;

    union {
        uint8_t scene_mode;

        struct {
            uint8_t work_mode     : 3;
            uint8_t user_gps_mode : 1; // 0 单点，1差分定诶
            uint8_t work_mode     : 4; // 1车载，2室内，3船载，4固定翼，5旋翼，6直升机
        };
    };

    uint8_t _reserved1[2];

    uint8_t  gps_cycle_cnt;
    uint16_t imu_timestamp_ms;

    union {
        uint8_t gps_info[4];

        // 周期1
        struct {
            uint8_t heading_exist    : 1;
            uint8_t utc_exist        : 1;
            uint8_t pv_valid         : 1;
            uint8_t heading_valid    : 1;
            uint8_t utc_valid        : 1;
            uint8_t outlfag_reserved : 3;
        };

        // 周期2
        uint32_t gps_timestamp;

        // 周期3~8

        // 周期9~11

        // 周期12
        struct {
            uint16_t gps_heading;
            uint16_t gps_reserved;
        };

        // 周期16
        struct {
            uint8_t  utc_satus;
            uint16_t utc_year;
            uint8_t  utc_month;
        };

        // 周期17
        struct {
            uint8_t utc_day;
            uint8_t utc_hour;
            uint8_t utc_minute;
            uint8_t utc_second;
        };
    };

    float temperautre_degC;

    uint16_t unknown_cnt;
    uint8_t  _reserved2;
    uint8_t  crc_bytes[2];
    float    pos[3];
    bool     start_fusion_flag;
    uint64_t err_cnt;
    float    freq;
} ps_gs02b_msg_t;

#pragma pack(pop)

#endif // _PA_GS02B_HPP_
