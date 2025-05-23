
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file ParameterUpdate.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT parameter_update_s {
#else
struct parameter_update_s {
#endif // __cplusplus
	uint64_t timestamp;
	uint32_t instance;
	uint32_t get_count;
	uint32_t set_count;
	uint32_t find_count;
	uint32_t export_count;
	uint16_t active;
	uint16_t changed;
	uint16_t custom_default;
	uint8_t _padding0[6]; // required for logger

#ifdef __cplusplus
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using ParameterUpdate = parameter_update_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(parameter_update);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const parameter_update_s& message);
#endif // __cplusplus
