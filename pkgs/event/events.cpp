/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <pthread.h>

#include <hrtimer.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/events.h>
#include <uORB/uORB.h>

static orb_advert_t    orb_event_pub       = nullptr;
static pthread_mutex_t publish_event_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint16_t        event_sequence{events::initial_event_sequence};

namespace events {

void send(EventType &event) {
    event.timestamp = hrt_absolute_time();

    // We need some synchronization here because:
    // - modifying orb_event_pub
    // - the update of event_sequence needs to be atomic
    // - we need to ensure ordering of the sequence numbers: the sequence we set here
    //   has to be the one published next.
    pthread_mutex_lock(&publish_event_mutex);
    event.event_sequence = ++event_sequence; // Set the sequence here so we're able to detect uORB queue overflows

    if (orb_event_pub != nullptr) {
        orb_publish(ORB_ID(event), orb_event_pub, &event);

    } else {
        orb_event_pub = orb_advertise_queue(ORB_ID(event), &event, event_s::ORB_QUEUE_LENGTH);
    }

    pthread_mutex_unlock(&publish_event_mutex);
}

} /* namespace events */
