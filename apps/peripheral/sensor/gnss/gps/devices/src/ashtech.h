/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/** @file ASHTECH protocol definitions */

#pragma once

#include "gps_helper.h"
#include "base_station.h"
#include "../../definitions.h"

#include <math.h>

class RTCMParsing;

#define ASHTECH_RECV_BUFFER_SIZE 512
#define ASH_RESPONSE_TIMEOUT     200 // ms, timeout for waiting for a response

class GPSDriverAshtech : public GPSBaseStationSupport {
public:
    /**
	 * @param heading_offset heading offset in radians [-pi, pi]. It is substracted from the measurement.
	 */
    GPSDriverAshtech(GPSCallbackPtr callback, void *callback_user, sensor_gps_s *gps_position,
                     satellite_info_s *satellite_info, float heading_offset = 0.f);

    virtual ~GPSDriverAshtech();

    int configure(unsigned &baudrate, const GPSConfig &config) override;

    int receive(unsigned timeout) override;

private:
    enum class AshtechBoard {
        trimble_mb_two,
        other
    };

    enum class NMEACommand {
        Acked,  // Command that returns a (N)Ack
        PRT,    // port config
        RID,    // board identification
        RECEIPT // board identification
    };

    enum class NMEACommandState {
        idle,
        waiting,
        nack,
        received
    };

    enum class NMEADecodeState {
        uninit,
        got_sync1,
        got_asteriks,
        got_first_cs_byte,
        decode_rtcm3
    };

    /**
	 * enable output of correction output
	 */
    void activateCorrectionOutput();

    void activateRTCMOutput(bool reduce_update_rate);

    void decodeInit(void);

    int handleMessage(int len);

    int parseChar(uint8_t b);

    /**
	 * receive data for at least the specified amount of time
	 */
    void receiveWait(unsigned timeout_min);

    void sendSurveyInStatusUpdate(bool active, bool valid, double latitude = static_cast<double>(NAN),
                                  double longitude = static_cast<double>(NAN), float altitude = NAN);

    /**
	 * Write a command and wait for a (N)Ack
	 * @return 0 on success, <0 otherwise
	 */
    int writeAckedCommand(const void *buf, int buf_length, unsigned timeout);

    int waitForReply(NMEACommand command, const unsigned timeout);

    bool _correction_output_activated{false};
    bool _configure_done{false};
    bool _got_pashr_pos_message{false}; /**< If we got a PASHR,POS message, we will ignore GGA messages */

    char _port{'A'};                    /**< port we are connected to (e.g. 'A') */

    uint8_t  _rx_buffer[ASHTECH_RECV_BUFFER_SIZE];
    uint16_t _rx_buffer_bytes{};
    uint64_t _last_timestamp_time{0};

    float _heading_offset;

    gps_abstime _survey_in_start{0};

    sensor_gps_s *_gps_position{nullptr};

    satellite_info_s *_satellite_info{nullptr};

    AshtechBoard _board{AshtechBoard::other}; /**< board we are connected to */

    NMEACommand _waiting_for_command;

    NMEACommandState _command_state{NMEACommandState::idle};

    NMEADecodeState _decode_state{NMEADecodeState::uninit};

    OutputMode _output_mode{OutputMode::GPS};

    RTCMParsing *_rtcm_parsing{nullptr};
};
