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

#include "WorkItem.hpp"
#include <hrtimer.h>

namespace nextpilot {

class WorkItemScheduled : public WorkItem {
public:
    bool Scheduled() {
        return !hrt_called(&_call);
    }

    /**
     * Schedule next run with a delay in microseconds.
     *
     * @param delay_us		The delay in microseconds.
     */
    void ScheduleDelayed(uint32_t delay_us);

    /**
     * Schedule repeating run with optional delay.
     *
     * @param interval_us		The interval in microseconds.
     * @param delay_us		The delay (optional) in microseconds.
     */
    void ScheduleOnInterval(uint32_t interval_us, uint32_t delay_us = 0);

    /**
     * Schedule next run at a specific time.
     *
     * @param time_us		The time in microseconds.
     */
    void ScheduleAt(hrt_abstime time_us);

    /**
     * Clear any scheduled work.
     */
    void ScheduleClear();

protected:
    WorkItemScheduled(const char *name, const wq_config_t &config) :
        WorkItem(name, config) {
    }
    virtual ~WorkItemScheduled() override;

    virtual void print_run_status() override;

private:
    virtual void Run() override = 0;

    static void schedule_trampoline(void *arg);

    hrt_call _call{};
};

} // namespace nextpilot
