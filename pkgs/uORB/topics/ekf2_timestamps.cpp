
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file Ekf2Timestamps.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/ekf2_timestamps.h>

constexpr char __orb_ekf2_timestamps_fields[] = "\x89 timestamp;\x83 airspeed_timestamp_rel;\x83 distance_sensor_timestamp_rel;\x83 optical_flow_timestamp_rel;\x83 vehicle_air_data_timestamp_rel;\x83 vehicle_magnetometer_timestamp_rel;\x83 visual_odometry_timestamp_rel;\x86[4] _padding0;";

ORB_DEFINE(ekf2_timestamps, struct ekf2_timestamps_s, 20, __orb_ekf2_timestamps_fields, static_cast<uint8_t>(ORB_ID::ekf2_timestamps));


void print_message(const orb_metadata *meta, const ekf2_timestamps_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
