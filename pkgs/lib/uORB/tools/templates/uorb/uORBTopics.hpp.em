@###############################################
@#
@# EmPy template for generating uORBTopics.hpp file
@# for logging purposes
@#
@###############################################
@# Start of Template
@#
@# Context:
@#  - topics (List) list of all topic names
@###############################################

/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

@{
msg_names = list(set([mn.replace(".msg", "") for mn in msgs])) # set() filters duplicates
msg_names.sort()
msgs_count = len(msg_names)

topics_count = len(topics)
topic_names_all = list(set(topics)) # set() filters duplicates
topic_names_all.sort()
}@

#pragma once

#include <stddef.h>
#include <uORB.h>
@[for msg_name in msg_names]@
#include <topics/@(msg_name).h>
@[end for]

static constexpr size_t ORB_TOPICS_COUNT{@(topics_count)};
static constexpr size_t orb_topics_count() { return ORB_TOPICS_COUNT; }

/*
 * Returns array of topics metadata
 */
extern const struct orb_metadata *const *orb_get_topics() __EXPORT;

enum class ORB_ID : uint8_t {
@[for idx, topic_name in enumerate(topic_names_all)]@
	@(topic_name) = @(idx),
@[end for]
	INVALID
};

const struct orb_metadata *get_orb_meta(ORB_ID id);
