/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __TONEALARM_H__
#define __TONEALARM_H__


/**
 * @file ToneAlarm.h
 *
 * Low Level Driver for the PX4 audio alarm port. Subscribes to
 * tune_control and plays notes on this architecture specific timer HW.
 */

#include <module/module_params.h>
#include <module/module_command.h>
#include <workq/WorkItemScheduled.hpp>
#include <circuit_breaker/circuit_breaker.h>
#include <tunes/tunes.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/tune_control.h>

#include <drivers/drv_tone_alarm.h>

#include <string.h>

using namespace nextpilot::workq;

class ToneAlarm : public ModuleCommand<ToneAlarm>, public WorkItemScheduled {
public:
    ToneAlarm();
    ~ToneAlarm() override;

    /** @see ModuleCommand */
    // static int task_spawn(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]) {
        return 0;
    };

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

private:
    static void InterruptStopNote(void *arg);

    int  init() override;
    void Run() override;

    hrt_call _hrt_call{};

    Tunes _tunes;

    hrt_abstime _next_note_time{0};

    uORB::SubscriptionCallbackWorkItem _tune_control_sub{this, ORB_ID(tune_control)};

    bool _circuit_break_initialized{false};
    bool _play_tone{false};
};

#endif // __TONEALARM_H__
