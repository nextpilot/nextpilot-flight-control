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

#include <stdint.h>

#include "mavlink_bridge_header.h"
#include <drivers/drv_hrt.h>
#include <px4_platform_common/atomic.h>
#include <px4_platform_common/events.h>
#include <pthread.h>

using namespace time_literals;

class Mavlink;

namespace events {

/**
 * @struct Event
 * Stores all data required for sending an event. This needs to be kept as small as possible,
 * so that we can buffer more events
 */
struct Event {
    uint32_t timestamp_ms;
    uint32_t id;
    uint16_t sequence;
    uint8_t  log_levels; ///< Log levels: 4 bits MSB: internal, 4 bits LSB: external
    uint8_t  arguments[MAX_ARGUMENTS_SIZE];
};

/**
 * @class EventBuffer
 * Event buffer that can be shared between threads and multiple SendProtocol instances.
 * All methods are thread-safe.
 */
class EventBuffer {
public:
    /**
     * Create an event buffer. Required memory: sizeof(Event) * capacity.
     * @param capacity maximum number of buffered events
     */
    EventBuffer(int capacity = 20);
    ~EventBuffer();

    int init();

    uint16_t get_latest_sequence() const {
        return _latest_sequence.load();
    }

    uint16_t get_oldest_sequence_after(uint16_t sequence) const;

    /**
     * Insert a new event. It's expect to have a later sequence number than the
     * already inserted events.
     */
    void insert_event(const Event &event);

    bool get_event(uint16_t sequence, Event &event) const;

    int size() const;

private:
    ::px4::atomic<uint16_t> _latest_sequence{events::initial_event_sequence};

    Event *_events{nullptr}; ///< stored events, ringbuffer
    int    _capacity;
    int    _next{0};         ///< next element to use
    int    _size{0};

    mutable pthread_mutex_t _mutex;
};

/**
 * @class SendProtocol
 * Handles sending of events
 */
class SendProtocol {
public:
    SendProtocol(EventBuffer &buffer, Mavlink &mavlink);

    /**
     * Handle sending of new events by checking the event buffer. Should be called
     * regularly.
     * @param now current time
     */
    void update(const hrt_abstime &now);

    /**
     * Handle mavlink_request_event_t message. Can be called from another thread than
     * the rest of the class and is therefore thread-safe.
     */
    void handle_request_event(const mavlink_message_t &msg) const;

    /**
     * Should be called whenever a GCS is connected
     */
    void on_gcs_connected();

private:
    void send_event(const Event &event) const;
    void send_current_sequence(const hrt_abstime &now, bool force_reset = false);

    static constexpr hrt_abstime current_sequence_interval{3_s};

    EventBuffer &_buffer;
    uint16_t     _latest_sequence;
    hrt_abstime  _last_current_sequence_sent{0};
    Mavlink     &_mavlink;
    bool         _first_event{true};
};

} /* namespace events */
