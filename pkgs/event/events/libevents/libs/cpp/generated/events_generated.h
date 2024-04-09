// auto-generated from events.h.jinja



#pragma once

#include <stdint.h>
#include <string.h>

#include <libevents_definitions.h>

namespace events
{
static constexpr int MAX_ARGUMENTS_SIZE = 25; ///< maximum number of bytes for all arguments

static_assert(MAX_ARGUMENTS_SIZE <= sizeof(EventType::arguments), "Argument size mismatch");

enum class LogLevel : uint8_t {
	Emergency = 0,
	Alert = 1,
	Critical = 2,
	Error = 3,
	Warning = 4,
	Notice = 5,
	Info = 6,
	Debug = 7,
	Protocol = 8,
	Disabled = 9,

	Count
};


enum class LogLevelInternal : uint8_t {
	Emergency = 0,
	Alert = 1,
	Critical = 2,
	Error = 3,
	Warning = 4,
	Notice = 5,
	Info = 6,
	Debug = 7,
	Protocol = 8,
	Disabled = 9,

	Count
};


using Log = LogLevel;
using LogInternal = LogLevelInternal;

struct LogLevels {
	LogLevels() {}
	LogLevels(Log external_level) : external(external_level), internal((LogInternal)external_level) {}
	LogLevels(Log external_level, LogInternal internal_level)
		: external(external_level), internal(internal_level) {}

	Log external{Log::Info};
	LogInternal internal{LogInternal::Info};
};

static inline LogInternal internalLogLevel(uint8_t log_levels) {
	return (LogInternal)(log_levels >> 4);
}

static inline Log externalLogLevel(uint8_t log_levels) {
	return (Log)(log_levels & 0xF);
}


namespace common // component id: 0
{
namespace enums
{

/**
 Calibration type
*/
enum class calibration_type_t : uint16_t {
	accel = 1, ///< Accelerometer
	mag = 2, ///< Magnetometer
	gyro = 4, ///< Gyroscope
	level = 8, ///< Level
	airspeed = 16, ///< Airspeed
	rc = 32, ///< RC

	_max = 32
};


static inline calibration_type_t operator|(calibration_type_t a, calibration_type_t b)
{
	return static_cast<calibration_type_t>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

static inline bool operator&(calibration_type_t a, calibration_type_t b)
{
	return static_cast<uint16_t>(a) & static_cast<uint16_t>(b);
}
/**
 Calibration Sides Bitfield
*/
enum class calibration_sides_t : uint8_t {
	tail_down = 1, ///< Tail Down
	nose_down = 2, ///< Nose Down
	left_side_down = 4, ///< Left Side Down
	right_side_down = 8, ///< Right Side Down
	upside_down = 16, ///< Upside Down
	down = 32, ///< Down

	_max = 32
};


static inline calibration_sides_t operator|(calibration_sides_t a, calibration_sides_t b)
{
	return static_cast<calibration_sides_t>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

static inline bool operator&(calibration_sides_t a, calibration_sides_t b)
{
	return static_cast<uint8_t>(a) & static_cast<uint8_t>(b);
}
/**
 Calibration Action/next step
*/
enum class calibration_action_t : uint8_t {
	already_completed = 0, ///< Side already completed, switch to one of the remaining sides
	next_orientation = 1, ///< Switch to next orientation
	rotate = 2, ///< Rotate as shown
	hold_still = 3, ///< Hold still

	_max = 3
};


/**
 Calibration Result
*/
enum class calibration_result_t : uint8_t {
	success = 0, ///< Success
	failed = 1, ///< Failed
	aborted = 2, ///< Aborted

	_max = 2
};



} // namespace enums
} // namespace common




namespace common // component id: 0
{

// Event group default


// Event group calibration

/**
 * Create event 'cal_progress'.
 * Message: Calibration progress: {2}%
 */
static inline EventType create_cal_progress(const LogLevels &log_levels, uint8_t proto_ver, int8_t progress, common::enums::calibration_type_t calibration_type, common::enums::calibration_sides_t required_sides)
{
	EventType event{};
	event.id = 1100;
	event.log_levels = ((uint8_t)log_levels.internal << 4) | (uint8_t)log_levels.external;
	memcpy(event.arguments+0, &proto_ver, sizeof(uint8_t));
	memcpy(event.arguments+1, &progress, sizeof(int8_t));
	memcpy(event.arguments+2, &calibration_type, sizeof(common::enums::calibration_type_t));
	memcpy(event.arguments+4, &required_sides, sizeof(common::enums::calibration_sides_t));
	return event;
}

/**
 * Decode event 'cal_progress'.
 * Message: Calibration progress: {2}%
 */
static inline void decode_cal_progress(const EventType &event, uint8_t &proto_ver, int8_t &progress, common::enums::calibration_type_t &calibration_type, common::enums::calibration_sides_t &required_sides)
{
	memcpy(&proto_ver, event.arguments+0, sizeof(uint8_t));
	memcpy(&progress, event.arguments+1, sizeof(int8_t));
	memcpy(&calibration_type, event.arguments+2, sizeof(common::enums::calibration_type_t));
	memcpy(&required_sides, event.arguments+4, sizeof(common::enums::calibration_sides_t));
}
/**
 * Create event 'cal_orientation_detected'.
 * Message: Orientation detected: {1}
 */
static inline EventType create_cal_orientation_detected(const LogLevels &log_levels, common::enums::calibration_sides_t orientation, common::enums::calibration_action_t action)
{
	EventType event{};
	event.id = 1101;
	event.log_levels = ((uint8_t)log_levels.internal << 4) | (uint8_t)log_levels.external;
	memcpy(event.arguments+0, &orientation, sizeof(common::enums::calibration_sides_t));
	memcpy(event.arguments+1, &action, sizeof(common::enums::calibration_action_t));
	return event;
}

/**
 * Decode event 'cal_orientation_detected'.
 * Message: Orientation detected: {1}
 */
static inline void decode_cal_orientation_detected(const EventType &event, common::enums::calibration_sides_t &orientation, common::enums::calibration_action_t &action)
{
	memcpy(&orientation, event.arguments+0, sizeof(common::enums::calibration_sides_t));
	memcpy(&action, event.arguments+1, sizeof(common::enums::calibration_action_t));
}
/**
 * Create event 'cal_orientation_done'.
 * Message: Orientation Complete: {1}, next step: {2}
 */
static inline EventType create_cal_orientation_done(const LogLevels &log_levels, common::enums::calibration_sides_t orientation, common::enums::calibration_action_t action)
{
	EventType event{};
	event.id = 1102;
	event.log_levels = ((uint8_t)log_levels.internal << 4) | (uint8_t)log_levels.external;
	memcpy(event.arguments+0, &orientation, sizeof(common::enums::calibration_sides_t));
	memcpy(event.arguments+1, &action, sizeof(common::enums::calibration_action_t));
	return event;
}

/**
 * Decode event 'cal_orientation_done'.
 * Message: Orientation Complete: {1}, next step: {2}
 */
static inline void decode_cal_orientation_done(const EventType &event, common::enums::calibration_sides_t &orientation, common::enums::calibration_action_t &action)
{
	memcpy(&orientation, event.arguments+0, sizeof(common::enums::calibration_sides_t));
	memcpy(&action, event.arguments+1, sizeof(common::enums::calibration_action_t));
}
/**
 * Create event 'cal_done'.
 * Message: Calibration Complete: {1}
 */
static inline EventType create_cal_done(const LogLevels &log_levels, common::enums::calibration_result_t result)
{
	EventType event{};
	event.id = 1103;
	event.log_levels = ((uint8_t)log_levels.internal << 4) | (uint8_t)log_levels.external;
	memcpy(event.arguments+0, &result, sizeof(common::enums::calibration_result_t));
	return event;
}

/**
 * Decode event 'cal_done'.
 * Message: Calibration Complete: {1}
 */
static inline void decode_cal_done(const EventType &event, common::enums::calibration_result_t &result)
{
	memcpy(&result, event.arguments+0, sizeof(common::enums::calibration_result_t));
}




enum class event_id_t : uint32_t {
	cal_progress = 1100,
	cal_orientation_detected = 1101,
	cal_orientation_done = 1102,
	cal_done = 1103,
};

} // namespace common


} // namespace events
