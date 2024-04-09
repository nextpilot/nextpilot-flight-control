#pragma once

#include <libevents_definitions.h>

#include <cstdint>
#include <functional>

namespace events
{
/**
 * @class ReceiveProtocol
 * Handles the MAVLink events protocol for receiving events. There should be one
 * instance per MAVLink source component id.
 */
class ReceiveProtocol
{
public:
    struct Callbacks {
        std::function<void(int num_events_lost)> error;  ///< lost events
        std::function<void(const mavlink_request_event_t&)> send_request_event_message;
        std::function<void(const mavlink_event_t&)> handle_event;
        std::function<void(int timeout_ms)> timer_control;  ///< control timer (single shot), timeout_ms <0 means to
                                                            ///< disable the timer
    };

    ReceiveProtocol(const Callbacks& callbacks, uint8_t our_system_id, uint8_t our_component_id, uint8_t system_id,
                    uint8_t component_id);

    void processMessage(const mavlink_message_t& msg);

    void timerEvent();

private:
    enum class SequenceComparison { older = -1, equal = 0, newer = 1 };

    void handleEvent(const mavlink_message_t& message);
    void handleCurrentEventSequence(const mavlink_message_t& message);
    void handleEventError(const mavlink_message_t& message);
    void checkTimestampReset(uint32_t timestamp);

    /**
     * Compare 2 sequence numbers with wrap-around handling.
     * @return 'equal' if equal, 'older' if incoming is old (duplicate), 'newer' if incoming is newer (dropped events)
     */
    SequenceComparison compareSequence(uint16_t expected_sequence, uint16_t incoming_sequence);

    void requestEvent(uint16_t sequence);

    const Callbacks _callbacks;

    uint16_t _latest_sequence;  ///< latest received sequence number
    bool _has_sequence{false};
    uint32_t _last_timestamp_ms{0};

    bool _has_current_sequence{false};
    uint32_t _latest_current_sequence;  ///< latest received sequence number via mavlink_current_event_sequence_t

    uint8_t _our_system_id;
    uint8_t _our_component_id;

    uint8_t _system_id;
    uint8_t _component_id;
};

} /* namespace events */
