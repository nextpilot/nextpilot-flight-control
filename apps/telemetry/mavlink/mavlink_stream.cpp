/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#define LOG_TAG "mavlink.stream"
#define LOG_LVL LOG_LVL_INFO

#include <stdlib.h>

#include "mavlink_stream.h"
#include "mavlink_main.h"

MavlinkStream::MavlinkStream(Mavlink *mavlink) :
    _mavlink(mavlink) {
    _last_sent = hrt_absolute_time();
}

/**
 * Update subscriptions and send message if necessary
 */
int MavlinkStream::update(const hrt_abstime &t) {
    update_data();

    // If the message has never been sent before we want
    // to send it immediately and can return right away
    if (_last_sent == 0) {
        // this will give different messages on the same run a different
        // initial timestamp which will help spacing them out
        // on the link scheduling
        if (send()) {
            _last_sent = hrt_absolute_time();

            if (!_first_message_sent) {
                _first_message_sent = true;
            }
        }

        return 0;
    }

    // One of the previous iterations sent the update
    // already before the deadline
    if (_last_sent > t) {
        return -1;
    }

    int64_t dt       = t - _last_sent;
    int     interval = _interval;

    if (!const_rate()) {
        interval /= _mavlink->get_rate_mult();
    }

    // We don't need to send anything if the inverval is 0. send() will be called manually.
    if (interval == 0) {
        return 0;
    }

    const bool unlimited_rate = interval < 0;

    // Send the message if it is due or
    // if it will overrun the next scheduled send interval
    // by 30% of the interval time. This helps to avoid
    // sending a scheduled message on average slower than
    // scheduled. Doing this at 50% would risk sending
    // the message too often as the loop runtime of the app
    // needs to be accounted for as well.
    // This method is not theoretically optimal but a suitable
    // stopgap as it hits its deadlines well (0.5 Hz, 50 Hz and 250 Hz)

    if (unlimited_rate || (dt > (interval - (_mavlink->get_main_loop_delay() / 10) * 3))) {
        // interval expired, send message

        // If the interval is non-zero and dt is smaller than 1.5 times the interval
        // do not use the actual time but increment at a fixed rate, so that processing delays do not
        // distort the average rate. The check of the maximum interval is done to ensure that after a
        // long time not sending anything, sending multiple messages in a short time is avoided.
        if (send()) {
            _last_sent = ((interval > 0) && ((int64_t)(1.5f * interval) > dt)) ? _last_sent + interval : t;

            if (!_first_message_sent) {
                _first_message_sent = true;
            }

            return 0;

        } else {
            return -1;
        }
    }

    return -1;
}
