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
 * @file nmea.h
 *
 * NMEA protocol definitions
 *
 * @author WeiPeng Guo <guoweipeng1990@sina.com>
 * @author Stone White <stone@thone.io>
 * @author Jose Jimenez-Berni <berni@ias.csic.es>
 *
 */

#pragma once

#include "gps_helper.h"
#include "../../definitions.h"
#include "unicore.h"

class RTCMParsing;

#define NMEA_RECV_BUFFER_SIZE 1024
#define NMEA_DEFAULT_BAUDRATE 115200

class GPSDriverNMEA : public GPSHelper {
public:
    /**
	 * @param heading_offset heading offset in radians [-pi, pi]. It is substracted from the measurement.
	 */
    GPSDriverNMEA(GPSCallbackPtr callback, void *callback_user,
                  sensor_gps_s     *gps_position,
                  satellite_info_s *satellite_info,
                  float             heading_offset = 0.f);

    virtual ~GPSDriverNMEA();

    int receive(unsigned timeout) override;
    int configure(unsigned &baudrate, const GPSConfig &config) override;

private:
    void handleHeading(float heading_deg, float heading_stddev_deg);
    void request_unicore_heading_message();

    UnicoreParser _unicore_parser;
    gps_abstime   _unicore_heading_received_last;

    enum class NMEADecodeState {
        uninit,
        got_sync1,
        got_asteriks,
        got_first_cs_byte,
        decode_rtcm3
    };

    void decodeInit(void);
    int  handleMessage(int len);
    int  parseChar(uint8_t b);

    int32_t read_int();
    double  read_float();
    char    read_char();

    sensor_gps_s     *_gps_position{nullptr};
    satellite_info_s *_satellite_info{nullptr};
    double            _last_POS_timeUTC{0};
    double            _last_VEL_timeUTC{0};
    double            _last_FIX_timeUTC{0};
    uint64_t          _last_timestamp_time{0};

    uint8_t _sat_num_gga{0};
    uint8_t _sat_num_gns{0};
    uint8_t _sat_num_gsv{0};
    uint8_t _sat_num_gpgsv{0};
    uint8_t _sat_num_glgsv{0};
    uint8_t _sat_num_gagsv{0};
    uint8_t _sat_num_gbgsv{0};
    uint8_t _sat_num_bdgsv{0};

    bool _clock_set{false};

    //  check if we got all basic essential packages we need
    bool _TIME_received{false};
    bool _POS_received{false};
    bool _ALT_received{false};
    bool _SVNUM_received{false};
    bool _SVINFO_received{false};
    bool _FIX_received{false};
    bool _DOP_received{false};
    bool _VEL_received{false};
    bool _EPH_received{false};
    bool _HEAD_received{false};

    NMEADecodeState _decode_state{NMEADecodeState::uninit};
    uint8_t         _rx_buffer[NMEA_RECV_BUFFER_SIZE]{};
    uint16_t        _rx_buffer_bytes{0};

    OutputMode _output_mode{OutputMode::GPS};

    RTCMParsing *_rtcm_parsing{nullptr};

    float _heading_offset;
};
