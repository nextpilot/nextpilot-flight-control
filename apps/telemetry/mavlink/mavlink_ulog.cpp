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
 * @file mavlink_ulog.cpp
 * ULog data streaming via MAVLink
 *
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#define LOG_TAG     "mavlink.ulog"
#define MODULE_NAME LOG_TAG

#include "mavlink_ulog.h"
#include <ulog/log.h>
#include <errno.h>
#include <mathlib/mathlib.h>

bool                MavlinkULog::_init     = false;
MavlinkULog        *MavlinkULog::_instance = nullptr;
struct rt_semaphore MavlinkULog::_lock;

MavlinkULog::MavlinkULog(int datarate, float max_rate_factor, uint8_t target_system, uint8_t target_component) :
    _target_system(target_system), _target_component(target_component),
    _max_rate_factor(max_rate_factor),
    _max_num_messages(math::max(1, (int)ceilf((_rate_calculation_delta_t / 1e6f) * _max_rate_factor * datarate / (MAVLINK_MSG_ID_LOGGING_DATA_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES)))),
    _current_rate_factor(max_rate_factor) {
    // make sure we won't read any old messages
    while (_ulog_stream_sub.update()) {
    }

    _waiting_for_initial_ack = true;
    _last_sent_time          = hrt_absolute_time(); //(ab)use this timestamp during initialization
    _next_rate_check         = _last_sent_time + _rate_calculation_delta_t;
}

MavlinkULog::~MavlinkULog() {
    perf_free(_msg_missed_ulog_stream_perf);
}

void MavlinkULog::start_ack_received() {
    if (_waiting_for_initial_ack) {
        _last_sent_time          = 0;
        _waiting_for_initial_ack = false;
        PX4_DEBUG("got logger ack");
    }
}

int MavlinkULog::handle_update(mavlink_channel_t channel) {
    static_assert(sizeof(ulog_stream_s::data) == MAVLINK_MSG_LOGGING_DATA_FIELD_DATA_LEN,
                  "Invalid uorb ulog_stream.data length");
    static_assert(sizeof(ulog_stream_s::data) == MAVLINK_MSG_LOGGING_DATA_ACKED_FIELD_DATA_LEN,
                  "Invalid uorb ulog_stream.data length");

    if (_waiting_for_initial_ack) {
        if (hrt_elapsed_time(&_last_sent_time) > 3e5) {
            PX4_WARN("no ack from logger (is it running?)");
            return -1;
        }

        return 0;
    }

    // check if we're waiting for an ACK
    if (_last_sent_time) {
        bool check_for_updates = false;

        if (_ack_received) {
            _last_sent_time   = 0;
            check_for_updates = true;

        } else {
            if (hrt_elapsed_time(&_last_sent_time) > ulog_stream_ack_s::ACK_TIMEOUT * 1000) {
                if (++_sent_tries > ulog_stream_ack_s::ACK_MAX_TRIES) {
                    return -ETIMEDOUT;

                } else {
                    PX4_DEBUG("re-sending ulog mavlink message (try=%i)", _sent_tries);
                    _last_sent_time = hrt_absolute_time();

                    const ulog_stream_s &ulog_data = _ulog_stream_sub.get();

                    mavlink_logging_data_acked_t msg;
                    msg.sequence             = ulog_data.msg_sequence;
                    msg.length               = ulog_data.length;
                    msg.first_message_offset = ulog_data.first_message_offset;
                    msg.target_system        = _target_system;
                    msg.target_component     = _target_component;
                    memcpy(msg.data, ulog_data.data, sizeof(msg.data));
                    mavlink_msg_logging_data_acked_send_struct(channel, &msg);
                }
            }
        }

        if (!check_for_updates) {
            return 0;
        }
    }

    while ((_current_num_msgs < _max_num_messages) && _ulog_stream_sub.updated()) {
        const unsigned last_generation = _ulog_stream_sub.get_last_generation();
        _ulog_stream_sub.update();

        if (_ulog_stream_sub.get_last_generation() != last_generation + 1) {
            perf_count(_msg_missed_ulog_stream_perf);
        }

        const ulog_stream_s &ulog_data = _ulog_stream_sub.get();

        if (ulog_data.timestamp > 0) {
            if (ulog_data.flags & ulog_stream_s::FLAGS_NEED_ACK) {
                _sent_tries     = 1;
                _last_sent_time = hrt_absolute_time();
                lock();
                _wait_for_ack_sequence = ulog_data.msg_sequence;
                _ack_received          = false;
                unlock();

                mavlink_logging_data_acked_t msg;
                msg.sequence             = ulog_data.msg_sequence;
                msg.length               = ulog_data.length;
                msg.first_message_offset = ulog_data.first_message_offset;
                msg.target_system        = _target_system;
                msg.target_component     = _target_component;
                memcpy(msg.data, ulog_data.data, sizeof(msg.data));
                mavlink_msg_logging_data_acked_send_struct(channel, &msg);

            } else {
                mavlink_logging_data_t msg;
                msg.sequence             = ulog_data.msg_sequence;
                msg.length               = ulog_data.length;
                msg.first_message_offset = ulog_data.first_message_offset;
                msg.target_system        = _target_system;
                msg.target_component     = _target_component;
                memcpy(msg.data, ulog_data.data, sizeof(msg.data));
                mavlink_msg_logging_data_send_struct(channel, &msg);
            }
        }

        ++_current_num_msgs;
    }

    // need to update the rate?
    hrt_abstime t = hrt_absolute_time();

    if (t > _next_rate_check) {
        if (_current_num_msgs < _max_num_messages) {
            _current_rate_factor = _max_rate_factor * (float)_current_num_msgs / _max_num_messages;

        } else {
            _current_rate_factor = _max_rate_factor;
        }

        _current_num_msgs = 0;
        _next_rate_check  = t + _rate_calculation_delta_t;
        PX4_DEBUG("current rate=%.3f (max=%i msgs in %.3fs)", (double)_current_rate_factor, _max_num_messages,
                  (double)(_rate_calculation_delta_t / 1e6));
    }

    return 0;
}

void MavlinkULog::initialize() {
    if (_init) {
        return;
    }

    rt_sem_init(&_lock, "ulog_lock", 1, RT_IPC_FLAG_PRIO);
    _init = true;
}

MavlinkULog *MavlinkULog::try_start(int datarate, float max_rate_factor, uint8_t target_system,
                                    uint8_t target_component) {
    MavlinkULog *ret    = nullptr;
    bool         failed = false;
    lock();

    if (!_instance) {
        ret = _instance = new MavlinkULog(datarate, max_rate_factor, target_system, target_component);

        if (!_instance) {
            failed = true;
        }
    }

    unlock();

    if (failed) {
        PX4_ERR("alloc failed");
    }

    return ret;
}

void MavlinkULog::stop() {
    lock();

    if (_instance) {
        delete _instance;
        _instance = nullptr;
    }

    unlock();
}

void MavlinkULog::handle_ack(mavlink_logging_ack_t ack) {
    lock();

    if (_instance) { // make sure stop() was not called right before
        if (_wait_for_ack_sequence == ack.sequence) {
            _ack_received = true;
            publish_ack(ack.sequence);
        }
    }

    unlock();
}

void MavlinkULog::publish_ack(uint16_t sequence) {
    ulog_stream_ack_s ack;
    ack.timestamp    = hrt_absolute_time();
    ack.msg_sequence = sequence;

    _ulog_stream_ack_pub.publish(ack);
}
