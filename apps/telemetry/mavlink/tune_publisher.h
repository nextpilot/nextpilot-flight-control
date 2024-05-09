/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <uORB/uORBPublication.hpp>
#include <hrtimer.h>
#include <tunes/tunes.h>

using namespace nextpilot;

class TunePublisher {
public:
    void set_tune_string(const char *tune, const hrt_abstime &now);
    void publish_next_tune(const hrt_abstime now);

private:
    static constexpr unsigned MAX_TUNE_LEN{248};

    Tunes       _tunes{};
    char        _tune_buffer[MAX_TUNE_LEN]{0};
    hrt_abstime _next_publish_time{0};

    uORB::Publication<tune_control_s> _tune_control_pub{ORB_ID(tune_control)};
};
