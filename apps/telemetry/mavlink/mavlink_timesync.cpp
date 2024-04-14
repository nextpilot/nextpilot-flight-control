/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file mavlink_timesync.cpp
 * Mavlink timesync implementation.
 *
 * @author Mohammed Kabir <mhkabir98@gmail.com>
 */

#include "mavlink_timesync.h"
#include "mavlink_main.h"

#include <stdlib.h>

MavlinkTimesync::MavlinkTimesync(Mavlink *mavlink) :
    _mavlink(mavlink) {
}

void MavlinkTimesync::handle_message(const mavlink_message_t *msg) {
    switch (msg->msgid) {
    case MAVLINK_MSG_ID_TIMESYNC: {
        mavlink_timesync_t tsync = {};
        mavlink_msg_timesync_decode(msg, &tsync);

        const uint64_t now = hrt_absolute_time();

        if (tsync.tc1 == 0) { // Message originating from remote system, timestamp and return it

            mavlink_timesync_t rsync;

            rsync.tc1 = now * 1000ULL;
            rsync.ts1 = tsync.ts1;

            mavlink_msg_timesync_send_struct(_mavlink->get_channel(), &rsync);

            return;

        } else if (tsync.tc1 > 0) { // Message originating from this system, compute time offset from it

            _timesync.update(now, tsync.tc1, tsync.ts1);
        }

        break;
    }

    case MAVLINK_MSG_ID_SYSTEM_TIME: {
        mavlink_system_time_t time;
        mavlink_msg_system_time_decode(msg, &time);

        timespec tv = {};
        px4_clock_gettime(CLOCK_REALTIME, &tv);

        // date -d @1234567890: Sat Feb 14 02:31:30 MSK 2009
        bool onb_unix_valid = (unsigned long long)tv.tv_sec > PX4_EPOCH_SECS;
        bool ofb_unix_valid = time.time_unix_usec > PX4_EPOCH_SECS * 1000000ULL;

        if (!onb_unix_valid && ofb_unix_valid) {
            tv.tv_sec  = time.time_unix_usec / 1000000ULL;
            tv.tv_nsec = (time.time_unix_usec % 1000000ULL) * 1000ULL;

            if (px4_clock_settime(CLOCK_REALTIME, &tv)) {
                PX4_ERR("[timesync] Failed setting realtime clock");
            }
        }

        break;
    }

    default:
        break;
    }
}
