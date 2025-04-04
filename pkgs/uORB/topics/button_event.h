
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file ButtonEvent.msg */

#pragma once

#include <uORB.h>

#ifndef __cplusplus
#define BUTTON_EVENT_ORB_QUEUE_LENGTH 2
#endif // __cplusplus


#ifdef __cplusplus
struct __EXPORT button_event_s {
#else
struct button_event_s {
#endif // __cplusplus
	uint64_t timestamp;
	bool triggered;
	uint8_t _padding0[7]; // required for logger

#ifdef __cplusplus
	static constexpr uint8_t ORB_QUEUE_LENGTH = 2;
#endif // __cplusplus
};

#ifdef __cplusplus
namespace nextpilot {
	namespace msg {
		using ButtonEvent = button_event_s;
	} // namespace msg
} // namespace nextpilot
#endif // __cplusplus

/* register this as object request broker structure */
ORB_DECLARE(button_event);
ORB_DECLARE(safety_button);

#ifdef __cplusplus
void print_message(const orb_metadata *meta, const button_event_s& message);
#endif // __cplusplus
