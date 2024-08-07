
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/* Auto-generated by genmsg_cpp from file LoggerStatus.msg */


#include <rtdbg.h>
#include <inttypes.h>
#include <hrtimer.h>
#include <topics/uORBTopics.hpp>
#include <topics/logger_status.h>

constexpr char __orb_logger_status_fields[] = "\x89 timestamp;\x8a total_written_kb;\x8a write_rate_kb_s;\x88 dropouts;\x88 message_gaps;\x88 buffer_used_bytes;\x88 buffer_size_bytes;\x86 type;\x86 backend;\x86 num_messages;\x86[5] _padding0;";

ORB_DEFINE(logger_status, struct logger_status_s, 35, __orb_logger_status_fields, static_cast<uint8_t>(ORB_ID::logger_status));


void print_message(const orb_metadata *meta, const logger_status_s& message)
{
	if (sizeof(message) != meta->o_size) {
		LOG_RAW("unexpected message size for %s: %zu != %i\n", meta->o_name, sizeof(message), meta->o_size);
		return;
	}
	// orb_print_message_internal(meta, &message, true);
}
