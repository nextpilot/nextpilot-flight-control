/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include "ToneAlarm.h"

#include <hrtime.h>
#include <uORB/Publication.hpp>

using namespace time_literals;

ToneAlarm::ToneAlarm() :
    WorkItemScheduled(MODULE_NAME, wq_configurations::hp_default) {
    // ensure ORB_ID(tune_control) is advertised with correct queue depth
    orb_advertise_queue(ORB_ID(tune_control), nullptr, tune_control_s::ORB_QUEUE_LENGTH);
}

ToneAlarm::~ToneAlarm() {
    ToneAlarmInterface::stop_note();
}

int ToneAlarm::init() {
    // NOTE: Implement hardware specific detail in the ToneAlarmInterface class implementation.
    ToneAlarmInterface::init();

    _tune_control_sub.set_interval_us(10_ms);

    if (!_tune_control_sub.registerCallback()) {
        LOG_E("callback registration failed");
        return RT_ERROR;
    }

    ScheduleNow();

    return 0;
}

void ToneAlarm::InterruptStopNote(void *arg) {
    ToneAlarmInterface::stop_note();
}

void ToneAlarm::Run() {
    // Check if circuit breaker is enabled.
    if (!_circuit_break_initialized) {
        if (circuit_breaker_enabled("CBRK_BUZZER", CBRK_BUZZER_KEY)) {
            request_stop();
        }

        _circuit_break_initialized = true;
    }

    if (should_exit()) {
        _tune_control_sub.unregisterCallback();
        exit_and_cleanup();
        return;
    }

    // Check for next tune_control when not currently playing
    if (_tune_control_sub.updated()) {
        tune_control_s tune_control;

        if (_tune_control_sub.copy(&tune_control)) {
            if (tune_control.timestamp > 0) {
                Tunes::ControlResult tune_result = _tunes.set_control(tune_control);

                switch (tune_result) {
                case Tunes::ControlResult::Success:
                    LOG_D("new tune %d", tune_control.tune_id);

                    if (tune_control.tune_override) {
                        // clear existing
                        ToneAlarmInterface::stop_note();
                        _next_note_time = 0;
                        hrt_cancel(&_hrt_call);
                    }

                    _play_tone = true;

#if (!defined(TONE_ALARM_TIMER) && !defined(GPIO_TONE_ALARM_GPIO)) || defined(DEBUG_BUILD)

                    switch (tune_control.tune_id) {
                    case tune_control_s::TUNE_ID_STARTUP:
                        LOG_I("startup tune");
                        break;

                    case tune_control_s::TUNE_ID_ERROR:
                        LOG_E("error tune");
                        break;

                    case tune_control_s::TUNE_ID_NOTIFY_POSITIVE:
                        LOG_I("notify positive");
                        break;

                    case tune_control_s::TUNE_ID_NOTIFY_NEUTRAL:
                        LOG_I("notify neutral");
                        break;

                    case tune_control_s::TUNE_ID_NOTIFY_NEGATIVE:
                        LOG_I("notify negative");
                        break;

                    case tune_control_s::TUNE_ID_ARMING_WARNING:
                        LOG_I("arming warning");
                        break;

                    case tune_control_s::TUNE_ID_BATTERY_WARNING_SLOW:
                        LOG_I("battery warning (slow)");
                        break;

                    case tune_control_s::TUNE_ID_BATTERY_WARNING_FAST:
                        LOG_I("battery warning (fast)");
                        break;

                    case tune_control_s::TUNE_ID_ARMING_FAILURE:
                        LOG_E("arming failure");
                        break;

                    case tune_control_s::TUNE_ID_SINGLE_BEEP:
                        LOG_I("beep");
                        break;

                    case tune_control_s::TUNE_ID_HOME_SET:
                        LOG_I("home set");
                        break;
                    }

#endif // (!TONE_ALARM_TIMER && !GPIO_TONE_ALARM_GPIO) || DEBUG_BUILD

                    break;

                case Tunes::ControlResult::WouldInterrupt:
                    // otherwise re-publish tune to process next
                    LOG_D("tune already playing, requeing tune: %d", tune_control.tune_id);
                    {
                        uORB::Publication<tune_control_s> tune_control_pub{ORB_ID(tune_control)};
                        tune_control.timestamp = hrt_absolute_time();
                        tune_control_pub.publish(tune_control);
                    }

                    break;


                case Tunes::ControlResult::InvalidTune:
                    LOG_W("Invalid tune: %d", tune_control.tune_id);
                    break;

                case Tunes::ControlResult::AlreadyPlaying:
                    // Do nothing
                    break;
                }
            }
        }
    }

    unsigned int frequency      = 0;
    unsigned int duration       = 0;
    unsigned int silence_length = 0;

    // Does an inter-note silence occur?
    if ((_next_note_time != 0) && (hrt_absolute_time() < _next_note_time)) {
        LOG_D("inter-note silence");
        ScheduleAt(_next_note_time);

    } else if (_play_tone && (_tunes.get_next_note(frequency, duration, silence_length) == Tunes::Status::Continue)) {
        LOG_D("Play frequency: %d, duration: %d us, silence: %d us", frequency, duration, silence_length);

        if (frequency > 0) {
            // Start playing the note.
            const hrt_abstime time_started = ToneAlarmInterface::start_note(frequency);

            if (time_started > 0) {
                // schedule stop with HRT
                hrt_call_at(&_hrt_call, time_started + duration, (hrt_callout)&InterruptStopNote, this);
                _next_note_time = time_started + duration + silence_length;

                // schedule next note
                ScheduleAt(_next_note_time);
            }

        } else {
            // A frequency of 0 corresponds to ToneAlarmInterface::stop_note()
            _next_note_time = hrt_absolute_time() + duration + silence_length;
            ToneAlarmInterface::stop_note();
            ScheduleAt(_next_note_time);
        }

    } else {
        LOG_D("stopping");
        ToneAlarmInterface::stop_note();
        _play_tone      = false;
        _next_note_time = 0;
    }

    // if done playing and a new tune_control is still available re-schedule
    if (!Scheduled() && _tune_control_sub.updated()) {
        ScheduleDelayed(_tunes.get_maximum_update_interval());
    }
}

int ToneAlarm::task_spawn(int argc, char *argv[]) {
    ToneAlarm *instance = new ToneAlarm();

    if (!instance) {
        LOG_E("alloc failed");
        return -1;
    }

    if (!instance->Init()) {
        delete instance;
        return RT_ERROR;
    }

    _object.store(instance);
    _task_id = task_id_is_work_queue;

    return RT_EOK;
}

int ToneAlarm::print_usage(const char *reason) {
    if (reason) {
        LOG_W("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
This module is responsible for the tone alarm.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("tone_alarm", "driver");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

extern "C" __EXPORT int tone_alarm_main(int argc, char *argv[]) {
    return ToneAlarm::main(argc, argv);
}
