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
 * @file mavlink_rate_limiter.h
 * Message rate limiter definition.
 *
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#ifndef MAVLINK_RATE_LIMITER_H_
#define MAVLINK_RATE_LIMITER_H_

#include <hrtimer.h>

class MavlinkRateLimiter {
private:
    hrt_abstime _last_sent{0};
    hrt_abstime _interval{1000000};

public:
    MavlinkRateLimiter() = default;
    MavlinkRateLimiter(unsigned int interval) :
        _interval(interval){};

    ~MavlinkRateLimiter() = default;

    void set_interval(unsigned int interval) {
        _interval = interval;
    }

    bool check(const hrt_abstime &t);
};

#endif /* MAVLINK_RATE_LIMITER_H_ */
