
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file VehicleAirData.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT vehicle_air_data_s {
#else
struct vehicle_air_data_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint64_t timestamp_sample;
	uint32_t baro_device_id;
	float baro_alt_meter;
	float baro_temp_celcius;
	float baro_pressure_pa;
	float rho;
	uint8_t calibration_count;
	uint8_t _padding0[3]; // required for logger

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using VehicleAirData = vehicle_air_data_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(vehicle_air_data);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const vehicle_air_data_s& message);
#endif // __cplusplus
