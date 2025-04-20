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


/**
 Calibration Sides Bitfield
*/
enum class calibration_sides_t : uint8_t {
	tail_down = 1, ///< Tail Down
	upside_down = 16, ///< Upside Down
	nose_down = 2, ///< Nose Down
	down = 32, ///< Down
	left_side_down = 4, ///< Left Side Down
	right_side_down = 8, ///< Right Side Down

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
 Calibration type
*/
enum class calibration_type_t : uint16_t {
	accel = 1, ///< Accelerometer
	airspeed = 16, ///< Airspeed
	mag = 2, ///< Magnetometer
	rc = 32, ///< RC
	gyro = 4, ///< Gyroscope
	level = 8, ///< Level

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

} // namespace enums
} // namespace common

namespace px4 // component id: 1
{
namespace enums
{

/**
 Reason for arming/disarming
*/
enum class arm_disarm_reason_t : uint8_t {
	transition_to_standby = 0, ///< Transition to standby
	rc_stick = 1, ///< RC
	failure_detector = 10, ///< failure detector
	shutdown = 11, ///< shutdown request
	unit_test = 12, ///< unit tests
	rc_button = 13, ///< RC (button)
	failsafe = 14, ///< failsafe
	rc_switch = 2, ///< RC (switch)
	command_internal = 3, ///< internal command
	command_external = 4, ///< external command
	mission_start = 5, ///< mission start
	auto_disarm_land = 6, ///< landing
	auto_disarm_preflight = 7, ///< auto preflight disarming
	kill_switch = 8, ///< kill switch
	lockdown = 9, ///< lockdown

	_max = 14
};


/**
 State of the main arming state machine
*/
enum class arming_state_t : uint8_t {
	init = 0, ///< Init
	standby = 1, ///< Standby
	armed = 2, ///< Armed
	standby_error = 3, ///< Standby Error
	shutdown = 4, ///< Shutdown
	inair_restore = 5, ///< In-air Restore

	_max = 5
};


/**
 Reason for battery fault
*/
enum class battery_fault_reason_t : uint8_t {
	deep_discharge = 0, ///< Battery has deep discharged
	voltage_spikes = 1, ///< Battery detected voltage spikes
	over_temperature = 10, ///< Battery is over-heating
	cell_fail = 2, ///< One or more battery cells have failed
	over_current = 3, ///< Battery reported over-current
	fault_temperature = 4, ///< Battery has reached a critical temperature which can result in a critical failure
	under_temperature = 5, ///< Battery temperature is below operating range
	incompatible_voltage = 6, ///< Vehicle voltage is not compatible with battery one
	incompatible_firmware = 7, ///< Battery firmware is not compatible with current autopilot firmware
	incompatible_model = 8, ///< Battery model is not supported by the system
	hardware_fault = 9, ///< Battery reported an hardware problem

	_max = 10
};


/**
 Smart battery modes of operation
*/
enum class battery_mode_t : uint8_t {
	unknown = 0, ///< unknown
	autodischarging = 1, ///< auto discharging towards storage level
	hotswap = 2, ///< hot-swap

	_max = 2
};


/**
 Bitfield for ESC failure reason
*/
enum class esc_fault_reason_t : uint8_t {
	over_current = 0, ///< detected over current
	over_voltage = 1, ///< detected over voltage
	motor_over_temp = 2, ///< Motor has reached a critical temperature
	over_rpm = 3, ///< Motor RPM is exceeding the limits
	inconsistent_cmd = 4, ///< received an invalid control command
	motor_stuck = 5, ///< Motor stalled
	failure_generic = 6, ///< detected a generic hardware failure
	motor_warn_temp = 7, ///< Motor is over-heating
	esc_warn_temp = 8, ///< is over-heating
	esc_over_temp = 9, ///< reached a critical temperature

	_max = 9
};


/**
 failsafe action
*/
enum class failsafe_action_t : uint8_t {
	none = 0, ///< 
	warn = 1, ///< warning
	terminate = 10, ///< terminate
	fallback_posctrl = 2, ///< fallback to position control
	fallback_altctrl = 3, ///< fallback to altitude control
	fallback_stabilized = 4, ///< fallback to stabilized
	hold = 5, ///< hold
	rtl = 6, ///< RTL
	land = 7, ///< land
	descend = 8, ///< descend
	disarm = 9, ///< disarm

	_max = 10
};


/**
 Cause for entering failsafe
*/
enum class failsafe_cause_t : uint8_t {
	generic = 0, ///< 
	manual_control_loss = 1, ///< manual control loss
	gcs_connection_loss = 2, ///< GCS connection loss
	low_battery_level = 3, ///< low battery level
	critical_battery_level = 4, ///< critical battery level
	emergency_battery_level = 5, ///< emergency battery level

	_max = 5
};


/**
 Reason for geofence violation
*/
enum class geofence_violation_reason_t : uint8_t {
	dist_to_home_exceeded = 0, ///< maximum distance to home exceeded
	max_altitude_exceeded = 1, ///< maximum altitude exceeded
	fence_violation = 2, ///< approaching or outside geofence

	_max = 2
};


/**
 Bitfield for subsystems & components
*/
enum class health_component_t : uint32_t {
	none = 1, ///< None
	logging = 1024, ///< Logging
	system = 1048576, ///< System
	remote_control = 128, ///< Remote Control (RC or Joystick)
	local_position_estimate = 131072, ///< Local position estimate
	magnetometer = 134217728, ///< Magnetometer
	optical_flow = 16, ///< Optical flow
	attitude_controller = 16384, ///< Attitude controller
	global_position_estimate = 16777216, ///< Global position estimate
	absolute_pressure = 2, ///< Absolute pressure
	battery = 2048, ///< Battery
	camera = 2097152, ///< Camera
	motors_escs = 256, ///< Motors/ESCs
	mission = 262144, ///< Mission
	accel = 268435456, ///< Accelerometer
	vision_position = 32, ///< Vision position estimate
	position_controller = 32768, ///< Position controller
	storage = 33554432, ///< Storage
	differential_pressure = 4, ///< Differential pressure
	communication_links = 4096, ///< Communication links
	gimbal = 4194304, ///< Gimbal
	utm = 512, ///< UTM
	avoidance = 524288, ///< Avoidance
	gyro = 536870912, ///< Gyroscope
	distance_sensor = 64, ///< Distance sensor
	attitude_estimate = 65536, ///< Attitude estimate
	parachute = 67108864, ///< Parachute
	gps = 8, ///< GPS
	rate_controller = 8192, ///< Rate controller
	payload = 8388608, ///< Payload

	_max = 536870912
};


static inline health_component_t operator|(health_component_t a, health_component_t b)
{
	return static_cast<health_component_t>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

static inline bool operator&(health_component_t a, health_component_t b)
{
	return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}
/**
 Navigation/flight mode group bits. All modes within a group share the same arming checks. For now we directly map group == mode.
*/
enum class navigation_mode_group_t : uint32_t {
	manual = 1, ///< Fully manual mode (w/o any controller support)
	acro = 1024, ///< Acro
	precland = 1048576, ///< Precision Land
	takeoff = 131072, ///< Takeoff
	loiter = 16, ///< Loiter
	offboard = 16384, ///< Offboard
	altctl = 2, ///< Altitude mode
	orbit = 2097152, ///< Orbit
	land = 262144, ///< Land
	rtl = 32, ///< RTL
	stab = 32768, ///< Stabilized
	posctl = 4, ///< Position mode
	vtol_takeoff = 4194304, ///< VTOL Takeoff
	follow_target = 524288, ///< Follow Target
	mission = 8, ///< Mission mode

	_max = 4194304
};


static inline navigation_mode_group_t operator|(navigation_mode_group_t a, navigation_mode_group_t b)
{
	return static_cast<navigation_mode_group_t>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

static inline bool operator&(navigation_mode_group_t a, navigation_mode_group_t b)
{
	return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
}
/**
 Flight mode
*/
enum class navigation_mode_t : uint8_t {
	manual = 0, ///< Manual
	altctl = 1, ///< Altitude control
	auto_takeoff = 10, ///< Takeoff
	auto_land = 11, ///< Land
	auto_follow_target = 12, ///< Follow Target
	auto_precland = 13, ///< Precision Landing
	orbit = 14, ///< Orbit
	auto_vtol_takeoff = 15, ///< Vtol Takeoff
	posctl = 2, ///< Position control
	unknown = 255, ///< [Unknown]
	auto_mission = 3, ///< Mission
	auto_loiter = 4, ///< Hold
	auto_rtl = 5, ///< RTL
	acro = 6, ///< Acro
	offboard = 7, ///< Offboard
	stab = 8, ///< Stabilized

	_max = 255
};


/**
 Bitfield for sensor failover reason
*/
enum class sensor_failover_reason_t : uint16_t {
	no_data = 1, ///< No data
	high_error_density = 16, ///< High Error Density
	stale_data = 2, ///< Stale data
	timeout = 4, ///< Timeout
	high_error_count = 8, ///< High Error Count

	_max = 16
};


static inline sensor_failover_reason_t operator|(sensor_failover_reason_t a, sensor_failover_reason_t b)
{
	return static_cast<sensor_failover_reason_t>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

static inline bool operator&(sensor_failover_reason_t a, sensor_failover_reason_t b)
{
	return static_cast<uint16_t>(a) & static_cast<uint16_t>(b);
}
/**
 Sensor type for failover reporting
*/
enum class sensor_type_t : uint8_t {
	accel = 0, ///< Accelerometer
	gyro = 1, ///< Gyroscope
	mag = 2, ///< Magnetometer

	_max = 2
};


/**
 Suggested actions for the user in case of a safety critical event is triggered
*/
enum class suggested_action_t : uint8_t {
	none = 0, ///< 
	land = 1, ///< Land now!
	reduce_throttle = 2, ///< Reduce throttle!

	_max = 2
};



} // namespace enums
} // namespace px4




namespace common // component id: 0
{

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

// Event group default





enum class event_id_t : uint32_t {
	cal_progress = 1100,
	cal_orientation_detected = 1101,
	cal_orientation_done = 1102,
	cal_done = 1103,
};

} // namespace common
namespace px4 // component id: 1
{




enum class event_id_t : uint32_t {
};

} // namespace px4


} // namespace events
