/*
 * This header defines the events::EventType type. In case mavlink is used, do
 * this:
 *
#include <mavlink_header>
namespace events
{
using EventType = mavlink_event_t;
} // namespace events
 *
 *
 * This header can also enable & configure debug output:
 * - LIBEVENTS_DEBUG_PRINTF
 * - LIBEVENTS_PARSER_DEBUG_PRINTF
 */

#define LIBEVENTS_PARSER_DEBUG_PRINTF printf

#pragma once

#include <cstdint>

namespace events
{
struct EventType {
    uint32_t id;
    uint8_t log_levels;
    uint8_t arguments[40];
};
}  // namespace events
