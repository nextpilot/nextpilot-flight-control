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
 * @file mavlink_timesync.h
 * Mavlink time synchroniser definition.
 *
 * @author Mohammed Kabir <mhkabir98@gmail.com>
 */

#pragma once

#include "mavlink_bridge_header.h"

#include <lib/timesync/Timesync.hpp>

class Mavlink;

class MavlinkTimesync {
public:
    explicit MavlinkTimesync(Mavlink *mavlink);
    ~MavlinkTimesync() = default;

    void handle_message(const mavlink_message_t *msg);

    /**
     * Convert remote timestamp to local hrt time (usec)
     * Use synchronised time if available, monotonic boot time otherwise
     */
    uint64_t sync_stamp(uint64_t usec) {
        return _timesync.sync_stamp(usec);
    }

private:
    Mavlink *const _mavlink;
    Timesync       _timesync{};
};
