/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "tune_publisher.h"
#include "string.h"
#include <px4_platform_common/log.h>

void TunePublisher::set_tune_string(const char *tune, const hrt_abstime &now) {
    // The tune string needs to be 0 terminated.
    const unsigned tune_len = strlen(tune);

    // We don't expect the tune string to be longer than what can come in via MAVLink including 0 termination.
    if (tune_len >= MAX_TUNE_LEN) {
        PX4_ERR("Tune string too long.");
        return;
    }

    strncpy(_tune_buffer, tune, MAX_TUNE_LEN);

    _tunes.set_string(_tune_buffer, tune_control_s::VOLUME_LEVEL_DEFAULT);

    _next_publish_time = now;
}

void TunePublisher::publish_next_tune(const hrt_abstime now) {
    if (_next_publish_time == 0) {
        // Nothing to play.
        return;
    }

    if (now < _next_publish_time) {
        // Too early, try again later.
        return;
    }

    unsigned frequency;
    unsigned duration;
    unsigned silence;
    uint8_t  volume;

    if (_tunes.get_next_note(frequency, duration, silence, volume) == Tunes::Status::Continue) {
        tune_control_s tune_control{};
        tune_control.frequency = static_cast<uint16_t>(frequency);
        tune_control.duration  = static_cast<uint32_t>(duration);
        tune_control.silence   = static_cast<uint32_t>(silence);
        tune_control.volume    = static_cast<uint8_t>(volume);
        tune_control.timestamp = hrt_absolute_time();
        _tune_control_pub.publish(tune_control);

        _next_publish_time = now + duration + silence;

    } else {
        // We're done, let's reset.
        _next_publish_time = 0;
    }
}
