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

#include <stdbool.h>
#include <stdint.h>

#include <geo/geo.h>

#include <hrtimer.h>
#include <uORB/uORBPublication.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/transponder_report.h>
#include <uORB/topics/vehicle_command.h>
#include <mavlink_log.h>
// #include <event/events.h>
// #include <px4_platform_common/board_common.h>
#include <containers/Array.hpp>

using namespace time_literals;

static constexpr uint8_t NAVIGATOR_MAX_TRAFFIC{10};

static constexpr uint8_t UTM_GUID_MSG_LENGTH{11};

static constexpr uint8_t UTM_CALLSIGN_LENGTH{9};

static constexpr uint64_t CONFLICT_WARNING_TIMEOUT{60_s};

static constexpr float TRAFFIC_TO_UAV_DISTANCE_EXTENSION{1000.0f};


struct traffic_data_s {
	double lat_traffic;
	double lon_traffic;
	float alt_traffic;
	float heading_traffic;
	float vxy_traffic;
	float vz_traffic;
	bool in_conflict;
};

struct traffic_buffer_s {
	nextpilot::container::Array<uint32_t, NAVIGATOR_MAX_TRAFFIC> icao_address {};
	nextpilot::container::Array<hrt_abstime, NAVIGATOR_MAX_TRAFFIC> timestamp {};
};

struct conflict_detection_params_s {
	float crosstrack_separation;
	float vertical_separation;
	int collision_time_threshold;
	uint8_t traffic_avoidance_mode;
};

enum class TRAFFIC_STATE {
	NO_CONFLICT = 0,
	ADD_CONFLICT = 1,
	REMIND_CONFLICT = 2,
	REMOVE_OLD_CONFLICT = 3,
	BUFFER_FULL = 4
};


class AdsbConflict
{
public:
	AdsbConflict() = default;
	~AdsbConflict() = default;

	void detect_traffic_conflict(double lat_now, double lon_now, float alt_now, float vx_now, float vy_now, float vz_now);

	int find_icao_address_in_conflict_list(uint32_t icao_address);

	void remove_icao_address_from_conflict_list(int traffic_index);

	void add_icao_address_from_conflict_list(uint32_t icao_address);

	void get_traffic_state();

	void set_conflict_detection_params(float crosstrack_separation, float vertical_separation,
					   int collision_time_threshold, uint8_t traffic_avoidance_mode);


	bool send_traffic_warning(int traffic_direction, int traffic_seperation, uint16_t tr_flags,
				  char uas_id[UTM_GUID_MSG_LENGTH], char tr_callsign[UTM_CALLSIGN_LENGTH], uint64_t uas_id_int);

	transponder_report_s _transponder_report{};

	bool handle_traffic_conflict();

	void fake_traffic(const char *const callsign, float distance, float direction, float traffic_heading,
			  float altitude_diff,
			  float hor_velocity, float ver_velocity, int emitter_type, uint32_t icao_address, double lat_uav, double lon_uav,
			  float &alt_uav);

	void run_fake_traffic(double &lat_uav, double &lon_uav,
			      float &alt_uav);

	bool _conflict_detected{false};

	TRAFFIC_STATE _traffic_state{TRAFFIC_STATE::NO_CONFLICT};

	conflict_detection_params_s _conflict_detection_params{};


protected:
	traffic_buffer_s _traffic_buffer;

private:

	orb_advert_t _mavlink_log_pub{nullptr};

	crosstrack_error_s _crosstrack_error{};


	transponder_report_s tr{};

	orb_advert_t fake_traffic_report_publisher = orb_advertise_queue(ORB_ID(transponder_report), &tr, (unsigned int)20);

	TRAFFIC_STATE _traffic_state_previous{TRAFFIC_STATE::NO_CONFLICT};

};
