/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/// @file mavlink_ftp_test.h
///	@author Don Gagne <don@thegagnes.com>

#pragma once

#include <unit_test.h>
#ifndef MAVLINK_FTP_UNIT_TEST
#include "../mavlink_bridge_header.h"
#else
#include <mavlink.h>
#endif
#include "../mavlink_ftp.h"

class MavlinkFtpTest : public UnitTest {
public:
    MavlinkFtpTest();
    virtual ~MavlinkFtpTest() = default;

    virtual bool run_tests(void);

    static void receive_message_handler_generic(const mavlink_file_transfer_protocol_t *ftp_req, void *worker_data);

    /// Worker data for stream handler
    struct BurstInfo {
        MavlinkFtpTest *ftp_test_class;
        int             burst_state;
        bool            single_packet_file;
        uint32_t        file_size;
        uint8_t        *file_bytes;
    };

    static void receive_message_handler_burst(const mavlink_file_transfer_protocol_t *ftp_req, void *worker_data);

    static const uint8_t serverSystemId    = 50; ///< System ID for server
    static const uint8_t serverComponentId = 1;  ///< Component ID for server
    static const uint8_t serverChannel     = 0;  ///< Channel to send to

    static const uint8_t clientSystemId    = 1; ///< System ID for client
    static const uint8_t clientComponentId = 0; ///< Component ID for client

    // We don't want any of these
    MavlinkFtpTest(const MavlinkFtpTest &);
    MavlinkFtpTest &operator=(const MavlinkFtpTest &);

private:
    virtual void _init(void);
    virtual void _cleanup(void);

    bool _create_test_files(void);
    bool _remove_test_files(void);
    bool _ack_test(void);
    bool _bad_opcode_test(void);
    bool _bad_datasize_test(void);
    bool _list_test(void);
    bool _list_eof_test(void);
    bool _open_badfile_test(void);
    bool _open_terminate_test(void);
    bool _terminate_badsession_test(void);
    bool _read_test(void);
    bool _read_badsession_test(void);
    bool _burst_test(void);
    bool _removedirectory_test(void);
    bool _createdirectory_test(void);
    bool _removefile_test(void);

    void _receive_message_handler_generic(const mavlink_file_transfer_protocol_t *ftp_req);
    bool _setup_ftp_msg(const MavlinkFTP::PayloadHeader *payload_header,
                        const uint8_t *data, const uint8_t data_len,
                        mavlink_message_t *msg);
    bool _decode_message(const mavlink_file_transfer_protocol_t *ftp_msg, const MavlinkFTP::PayloadHeader **payload);
    bool _send_receive_msg(MavlinkFTP::PayloadHeader        *payload_header,
                           const uint8_t                    *data,
                           const size_t                      data_len,
                           const MavlinkFTP::PayloadHeader **payload_reply);
    void _cleanup_microsd(void);

    /// A single download test case
    struct DownloadTestCase {
        const char    *file;
        const uint16_t length;
        bool           singlePacketRead;
        bool           exactlyFillPacket;
    };

    /// The set of test cases for download testing
    static const DownloadTestCase _rgDownloadTestCases[];

    /// States for stream download handler
    enum {
        burst_state_first_ack,
        burst_state_last_ack,
        burst_state_nak_eof,
        burst_state_complete
    };

    bool _receive_message_handler_burst(const mavlink_file_transfer_protocol_t *ftp_req, BurstInfo *burst_info);

    MavlinkFTP *_ftp_server;
    uint16_t    _expected_seq_number;

    mavlink_file_transfer_protocol_t _reply_msg;

    static const char _unittest_microsd_dir[];
    static const char _unittest_microsd_file[];
};

bool mavlink_ftp_test(void);
