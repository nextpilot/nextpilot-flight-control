
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file SensorCorrection.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT sensor_correction_s {
#else
struct sensor_correction_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint32_t gyro_device_ids[4];
	float gyro_temperature[4];
	float gyro_offset_0[3];
	float gyro_offset_1[3];
	float gyro_offset_2[3];
	float gyro_offset_3[3];
	uint32_t accel_device_ids[4];
	float accel_temperature[4];
	float accel_offset_0[3];
	float accel_offset_1[3];
	float accel_offset_2[3];
	float accel_offset_3[3];
	uint32_t baro_device_ids[4];
	float baro_temperature[4];
	float baro_offset_0;
	float baro_offset_1;
	float baro_offset_2;
	float baro_offset_3;

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using SensorCorrection = sensor_correction_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(sensor_correction);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const sensor_correction_s& message);
#endif // __cplusplus
