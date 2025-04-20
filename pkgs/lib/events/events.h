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

// Console event output can be enabled if needed.
// Note: this will only print custom (PX4) events, not from common.
// Helpful for debugging and to ensure the system is not spammed with events.
// It does not print arguments.
#if 0
#include <ulog/log.h>
#define CONSOLE_PRINT_EVENT(log_level, id, str) PX4_INFO_RAW("Event 0x%08" PRIx32 ": %s\n", id, str)
#else
#define CONSOLE_PRINT_EVENT(log_level, id, str)
#endif

#include <events/events_generated.h>
#include <events/libevents_definitions.h>
#include <stdint.h>

namespace events {

namespace util {

// source: https://gist.github.com/ruby0x1/81308642d0325fd386237cfa3b44785c
constexpr uint32_t        val_32_const   = 0x811c9dc5;
constexpr uint32_t        prime_32_const = 0x1000193;
constexpr uint64_t        val_64_const   = 0xcbf29ce484222325;
constexpr uint64_t        prime_64_const = 0x100000001b3;
inline constexpr uint32_t hash_32_fnv1a_const(const char *const str, const uint32_t value = val_32_const) noexcept {
    return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

template <typename T>
inline constexpr void fillEventArguments(uint8_t *buf, T arg) {
    // This assumes we're on little-endian
    memcpy(buf, &arg, sizeof(T));
}

template <typename T, typename... Args>
inline constexpr void fillEventArguments(uint8_t *buf, T arg, Args... args) {
    fillEventArguments(buf, arg);
    fillEventArguments(buf + sizeof(T), args...);
}

constexpr unsigned sizeofArguments() {
    return 0;
}

template <typename T, typename... Args>
constexpr unsigned sizeofArguments(const T &t, const Args &...args) {
    return sizeof(T) + sizeofArguments(args...);
}

} // namespace util

/**
 * publish/send an event
 */
void send(EventType &event);

/**
 * Generate event ID from an event name
 */
template <size_t N>
constexpr uint32_t ID(const char (&name)[N]) {
    // Note: the generated ID must match with the python generator under Tools/px4events
    uint32_t component_id = 1u << 24; // autopilot component
    return (0xffffff & util::hash_32_fnv1a_const(name)) | component_id;
}

template <typename... Args>
inline void send(uint32_t id, const LogLevels &log_levels, const char *message, Args... args) {
    EventType e{};
    e.log_levels = ((uint8_t)log_levels.internal << 4) | (uint8_t)log_levels.external;
    e.id         = id;
    static_assert(util::sizeofArguments(args...) <= sizeof(e.arguments), "Too many arguments");
    util::fillEventArguments((uint8_t *)e.arguments, args...);
    CONSOLE_PRINT_EVENT(e.log_level_external, e.id, message);
    send(e);
}

inline void send(uint32_t id, const LogLevels &log_levels, const char *message) {
    EventType e{};
    e.log_levels = ((uint8_t)log_levels.internal << 4) | (uint8_t)log_levels.external;
    e.id         = id;
    CONSOLE_PRINT_EVENT(e.log_level_external, e.id, message);
    send(e);
}

static constexpr uint16_t initial_event_sequence = 65535 - 10; // initialize with a high number so it wraps soon

} // namespace events
