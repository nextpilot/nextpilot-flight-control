
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file VehicleAngularVelocity.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/vehicle_angular_velocity.h>

constexpr char __orb_vehicle_angular_velocity_fields[] = "\x89 timestamp;\x89 timestamp_sample;\x8a[3] xyz;\x8a[3] xyz_derivative;";

ORB_DEFINE(vehicle_angular_velocity, struct vehicle_angular_velocity_s, 40, __orb_vehicle_angular_velocity_fields, static_cast<uint8_t>(ORB_ID::vehicle_angular_velocity));
ORB_DEFINE(vehicle_angular_velocity_groundtruth, struct vehicle_angular_velocity_s, 40, __orb_vehicle_angular_velocity_fields, static_cast<uint8_t>(ORB_ID::vehicle_angular_velocity_groundtruth));


void print_message(const orb_metadata *meta, const vehicle_angular_velocity_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
