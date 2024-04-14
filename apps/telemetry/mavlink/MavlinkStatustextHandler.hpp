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

#include "mavlink_bridge_header.h"
#include <uORB/topics/log_message.h>
#include <stdint.h>

class MavlinkStatustextHandler {
public:
    bool should_publish_previous(const mavlink_statustext_t &msg_statustext);
    bool should_publish_current(const mavlink_statustext_t &msg_statustext, const uint64_t &now);

    const log_message_s &log_message() const {
        return _log_msg;
    };

private:
    log_message_s _log_msg{}; // required global for multi chunk statustexts
    int           _last_log_chunk_seq{-1};
    uint16_t      _last_log_id{0}; // non-zero means message has not been published
};
