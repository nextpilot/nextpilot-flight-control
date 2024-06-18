/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include "mavlink_rate_limiter.h"

bool MavlinkRateLimiter::check(const hrt_abstime &t) {
    uint64_t dt = t - _last_sent;

    if (dt > 0 && dt >= _interval) {
        _last_sent = t;
        return true;
    }

    return false;
}
