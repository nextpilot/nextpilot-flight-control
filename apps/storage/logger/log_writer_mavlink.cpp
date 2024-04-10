/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "log_writer_mavlink.h"
#include "messages.h"

#include <hrtimer.h>
#include <mathlib/mathlib.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>
#include <cstring>

namespace px4 {
namespace logger {

LogWriterMavlink::LogWriterMavlink() {
    _ulog_stream_data.length = 0;
}

bool LogWriterMavlink::init() {
    return true;
}

LogWriterMavlink::~LogWriterMavlink() {
    if (_ulog_stream_ack_sub >= 0) {
        orb_unsubscribe(_ulog_stream_ack_sub);
    }
}

void LogWriterMavlink::start_log() {
    if (_ulog_stream_ack_sub == -1) {
        _ulog_stream_ack_sub = orb_subscribe(ORB_ID(ulog_stream_ack));
    }

    // make sure we don't get any stale ack's by doing an orb_copy
    ulog_stream_ack_s ack;
    orb_copy(ORB_ID(ulog_stream_ack), _ulog_stream_ack_sub, &ack);

    _ulog_stream_data.msg_sequence         = 0;
    _ulog_stream_data.length               = 0;
    _ulog_stream_data.first_message_offset = 0;

    _is_started = true;
}

void LogWriterMavlink::stop_log() {
    _ulog_stream_data.length = 0;
    _is_started              = false;
}

int LogWriterMavlink::write_message(void *ptr, size_t size) {
    if (!is_started()) {
        return 0;
    }

    const uint8_t data_len = (uint8_t)sizeof(_ulog_stream_data.data);
    uint8_t      *ptr_data = (uint8_t *)ptr;

    if (_ulog_stream_data.first_message_offset == 255) {
        _ulog_stream_data.first_message_offset = _ulog_stream_data.length;
    }

    while (size > 0) {
        size_t send_len = math::min((size_t)data_len - _ulog_stream_data.length, size);
        memcpy(_ulog_stream_data.data + _ulog_stream_data.length, ptr_data, send_len);
        _ulog_stream_data.length += send_len;
        ptr_data += send_len;
        size -= send_len;

        if (_ulog_stream_data.length >= data_len) {
            if (publish_message()) {
                return -2;
            }
        }
    }

    return 0;
}

void LogWriterMavlink::set_need_reliable_transfer(bool need_reliable) {
    if (!need_reliable && _need_reliable_transfer) {
        if (_ulog_stream_data.length > 0) {
            // make sure to send previous data using reliable transfer
            publish_message();
        }
    }

    _need_reliable_transfer = need_reliable;
}

int LogWriterMavlink::publish_message() {
    _ulog_stream_data.timestamp = hrt_absolute_time();
    _ulog_stream_data.flags     = 0;

    if (_need_reliable_transfer) {
        _ulog_stream_data.flags = _ulog_stream_data.FLAGS_NEED_ACK;
    }

    _ulog_stream_pub.publish(_ulog_stream_data);

    if (_need_reliable_transfer) {
        // we need to wait for an ack. Note that this blocks the main logger thread, so if a file logging
        // is already running, it will miss samples.
        px4_pollfd_struct_t fds[1];
        fds[0].fd            = _ulog_stream_ack_sub;
        fds[0].events        = POLLIN;
        bool      got_ack    = false;
        const int timeout_ms = ulog_stream_ack_s::ACK_TIMEOUT * ulog_stream_ack_s::ACK_MAX_TRIES;

        hrt_abstime started = hrt_absolute_time();

        do {
            int ret = px4_poll(fds, sizeof(fds) / sizeof(fds[0]), timeout_ms);

            if (ret <= 0) {
                break;
            }

            if (fds[0].revents & POLLIN) {
                ulog_stream_ack_s ack;
                orb_copy(ORB_ID(ulog_stream_ack), _ulog_stream_ack_sub, &ack);

                if (ack.msg_sequence == _ulog_stream_data.msg_sequence) {
                    got_ack = true;
                }

            } else {
                break;
            }
        } while (!got_ack && hrt_elapsed_time(&started) / 1000 < timeout_ms);

        if (!got_ack) {
            PX4_ERR("Ack timeout. Stopping mavlink log");
            stop_log();
            return -2;
        }

        PX4_DEBUG("got ack in %i ms", (int)(hrt_elapsed_time(&started) / 1000));
    }

    _ulog_stream_data.msg_sequence++;
    _ulog_stream_data.length               = 0;
    _ulog_stream_data.first_message_offset = 255;
    return 0;
}

}
} // namespace px4::logger
