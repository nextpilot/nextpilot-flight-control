/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "MavlinkStatustextHandler.hpp"
#include <mathlib/mathlib.h>

bool MavlinkStatustextHandler::should_publish_previous(const mavlink_statustext_t &msg_statustext) {
    // Check if the previous message has not been published yet. This can
    // happen if the last chunk has been dropped, let's publish what we have.
    if (_last_log_id > 0 && msg_statustext.id != _last_log_id) {
        // We add a note that a part is missing at the end.
        const size_t offset = strlen(_log_msg.text);
        strncpy(_log_msg.text + offset, "[ missing ... ]",
                sizeof(_log_msg.text) - offset);
        _log_msg.text[sizeof(_log_msg.text) - 1] = '\0';

        return true;
    }

    return false;
}

bool MavlinkStatustextHandler::should_publish_current(const mavlink_statustext_t &msg_statustext, const uint64_t &now) {
    if (msg_statustext.id > 0) {
        // Multi statustext message.
        if (_last_log_id != msg_statustext.id) {
            // On the first one arriving, we save the timestamp and severity.
            _log_msg.timestamp = now;
            _log_msg.severity  = msg_statustext.severity;
        }

        if (msg_statustext.chunk_seq == 0) {
            // We start from 0 with a new message.
            strncpy(_log_msg.text, msg_statustext.text,
                    math::min(sizeof(_log_msg.text), sizeof(msg_statustext.text)));
            _log_msg.text[sizeof(msg_statustext.text)] = '\0';

        } else {
            if (msg_statustext.chunk_seq != _last_log_chunk_seq + 1) {
                const size_t offset = strlen(_log_msg.text);
                strncpy(_log_msg.text + offset, "[ missing ... ]",
                        sizeof(_log_msg.text) - offset);
                _log_msg.text[sizeof(_log_msg.text) - offset - 1] = '\0';
            }

            // We add a consecutive chunk.
            const size_t offset     = strlen(_log_msg.text);
            const size_t max_to_add = math::min(sizeof(_log_msg.text) - offset - 1, sizeof(msg_statustext.text));
            strncpy(_log_msg.text + offset, msg_statustext.text, max_to_add);
            _log_msg.text[math::min(offset + max_to_add, sizeof(_log_msg.text) - 1)] = '\0';
        }

        _last_log_chunk_seq = msg_statustext.chunk_seq;

        const bool publication_message_is_full = sizeof(_log_msg.text) - 1 == strlen(_log_msg.text);
        const bool found_zero_termination      = strnlen(msg_statustext.text,
                                                         sizeof(msg_statustext.text)) < sizeof(msg_statustext.text);

        if (publication_message_is_full || found_zero_termination) {
            _last_log_id        = 0;
            _last_log_chunk_seq = -1;
            return true;

        } else {
            _last_log_id = msg_statustext.id;
            return false;
        }

    } else {
        // Single statustext message.
        _log_msg.timestamp = now;
        _log_msg.severity  = msg_statustext.severity;

        static_assert(sizeof(_log_msg.text) > sizeof(msg_statustext.text),
                      "_log_msg.text not big enough to hold msg_statustext.text");

        strncpy(_log_msg.text, msg_statustext.text,
                math::min(sizeof(_log_msg.text),
                          sizeof(msg_statustext.text)));

        // We need to 0-terminate after the copied text which does not have to be
        // 0-terminated on the wire.
        _log_msg.text[sizeof(msg_statustext.text) - 1] = '\0';

        _last_log_id = 0;
        return true;
    }
}
