
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file VehicleAttitude.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT vehicle_attitude_s {
#else
struct vehicle_attitude_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint64_t timestamp_sample;
	float q[4];
	float delta_q_reset[4];
	uint8_t quat_reset_counter;
	uint8_t _padding0[7]; // required for logger

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using VehicleAttitude = vehicle_attitude_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(vehicle_attitude);
ORB_DECLARE(vehicle_attitude_groundtruth);
ORB_DECLARE(external_ins_attitude);
ORB_DECLARE(estimator_attitude);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const vehicle_attitude_s& message);
#endif // __cplusplus
