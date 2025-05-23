
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file SensorGnssRelative.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/sensor_gnss_relative.h>

constexpr char __orb_sensor_gnss_relative_fields[] = "\x89 timestamp;\x89 timestamp_sample;\x89 time_utc_usec;\x88 device_id;\x8a[3] position;\x8a[3] position_accuracy;\x8a heading;\x8a heading_accuracy;\x8a position_length;\x8a accuracy_length;\x87 reference_station_id;\x8c gnss_fix_ok;\x8c differential_solution;\x8c relative_position_valid;\x8c carrier_solution_floating;\x8c carrier_solution_fixed;\x8c moving_base_mode;\x8c reference_position_miss;\x8c reference_observations_miss;\x8c heading_valid;\x8c relative_position_normalized;";

ORB_DEFINE(sensor_gnss_relative, struct sensor_gnss_relative_s, 80, __orb_sensor_gnss_relative_fields, static_cast<uint8_t>(ORB_ID::sensor_gnss_relative));


void print_message(const orb_metadata *meta, const sensor_gnss_relative_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
