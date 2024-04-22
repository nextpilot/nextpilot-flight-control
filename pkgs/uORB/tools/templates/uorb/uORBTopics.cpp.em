@###############################################
@#
@# EmPy template for generating uORBTopics.cpp file
@# for logging purposes
@#
@###############################################
@# Start of Template
@#
@# Context:
@#  - msgs (List) list of all msg files
@#  - multi_topics (List) list of all multi-topic names
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

#include <uORB.h>
#include <topics/uORBTopics.hpp>
@{
msg_names = list(set([mn.replace(".msg", "") for mn in msgs])) # set() filters duplicates
msg_names.sort()
msgs_count = len(msg_names)

topic_names = list(set(topics)) # set() filters duplicates
topic_names.sort()
topics_count = len(topics)
}@

const constexpr struct orb_metadata *const uorb_topics_list[ORB_TOPICS_COUNT] = {
@[for idx, topic_name in enumerate(topic_names, 1)]@
	ORB_ID(@(topic_name))@[if idx != topic_names], @[end if]
@[end for]
};

const struct orb_metadata *const *orb_get_topics()
{
	return uorb_topics_list;
}

const struct orb_metadata *get_orb_meta(ORB_ID id)
{
	if (id == ORB_ID::INVALID) {
		return nullptr;
	}

	return uorb_topics_list[static_cast<uint8_t>(id)];
}
