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
 * @file beep.cpp
 *
 * @author CUAVcaijie <caijie@cuav.net>
 */

#include "beep.hpp"

#include <lib/circuit_breaker/circuit_breaker.h>

UavcanBeepController::UavcanBeepController(uavcan::INode &node) :
    _beep_pub(node),
    _timer(node) {
    _beep_pub.setPriority(uavcan::TransferPriority::MiddleLower);
}

int UavcanBeepController::init() {
    // don't initialize if CBRK_BUZZER circuit breaker is enabled.
    if (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_KEY)) {
        return 0;
    }

    /*
     * Setup timer and call back function for periodic updates
     */
    if (!_timer.isRunning()) {
        _timer.setCallback(TimerCbBinder(this, &UavcanBeepController::periodic_update));
        _timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(1000 / MAX_RATE_HZ));
    }

    return 0;
}

void UavcanBeepController::periodic_update(const uavcan::TimerEvent &) {
    if (_tune_control_sub.updated()) {
        _tune_control_sub.copy(&_tune);

        if (_tune.timestamp > 0) {
            _tunes.set_control(_tune);
        }
    }

    const hrt_abstime timestamp_now = hrt_absolute_time();

    if ((timestamp_now - _interval_timestamp <= _duration)) {
        return;
    }

    _interval_timestamp = timestamp_now;

    if (_silence_length > 0) {
        _duration       = _silence_length;
        _silence_length = 0;

    } else if (_tunes.get_next_note(_frequency, _duration, _silence_length) == Tunes::Status::Continue) {
        // Start playing the note.
        // A frequency of 0 corresponds to ToneAlarmInterface::stop_note()
        uavcan::equipment::indication::BeepCommand cmd{};
        cmd.frequency = _frequency;
        cmd.duration  = _duration / 1000000.f;
        _beep_pub.broadcast(cmd);
    }
}
