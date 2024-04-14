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

/// @file mavlink_log_handler.h
/// @author px4dev, Gus Grubba <mavlink@grubba.com>

#include <dirent.h>
#include <queue.h>
#include <time.h>
#include <stdio.h>
#include <cstdbool>
#include <drivers/drv_hrt.h>

#include "mavlink_bridge_header.h"

class Mavlink;

// MAVLink LOG_* Message Handler
class MavlinkLogHandler {
public:
    MavlinkLogHandler(Mavlink *mavlink);
    ~MavlinkLogHandler();

    // Handle possible LOG message
    void handle_message(const mavlink_message_t *msg);

    /**
     * Handle sending of messages. Call this regularly at a fixed frequency.
     * @param t current time
     */
    void send();

    unsigned get_size();

private:
    enum class LogHandlerState {
        Inactive,   // There is no active action of log handler
        Idle,       // The log handler is not sending list/data, but list has been sent
        Listing,    // File list is being send
        SendingData // File Data is being send
    };
    void _log_message(const mavlink_message_t *msg);
    void _log_request_list(const mavlink_message_t *msg);
    void _log_request_data(const mavlink_message_t *msg);
    void _log_request_erase(const mavlink_message_t *msg);
    void _log_request_end(const mavlink_message_t *msg);

    void        _reset_list_helper();
    void        _init_list_helper();
    bool        _get_session_date(const char *path, const char *dir, time_t &date);
    void        _scan_logs(FILE *f, const char *dir, time_t &date);
    bool        _get_log_time_size(const char *path, const char *file, time_t &date, uint32_t &size);
    static void _delete_all(const char *dir);
    bool        _get_entry(int idx, uint32_t &size, uint32_t &date, char *filename = 0, int filename_len = 0);
    bool        _open_for_transmit();
    size_t      _get_log_data(uint8_t len, uint8_t *buffer);
    void        _close_and_unlink_files();

    size_t _log_send_listing();
    size_t _log_send_data();

    LogHandlerState _current_status{LogHandlerState::Inactive};
    Mavlink        *_mavlink;

    int _next_entry{0};
    int _last_entry{0};
    int _log_count{0};

    uint16_t _current_log_index{UINT16_MAX};
    uint32_t _current_log_size{0};
    uint32_t _current_log_data_offset{0};
    uint32_t _current_log_data_remaining{0};
    FILE    *_current_log_filep{nullptr};
    char     _current_log_filename[128]; // TODO: consider to allocate on runtime
};
