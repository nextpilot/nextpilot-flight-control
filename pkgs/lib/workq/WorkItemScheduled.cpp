/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "WorkItemScheduled"

#include <WorkItemScheduled.hpp>
#include <inttypes.h>

namespace nextpilot::workq {

WorkItemScheduled::~WorkItemScheduled() {
    if (_call.arg != nullptr) {
        ScheduleClear();
    }
}

void WorkItemScheduled::schedule_trampoline(void *arg) {
    WorkItemScheduled *item = static_cast<WorkItemScheduled *>(arg);
    item->ScheduleNow();
}

void WorkItemScheduled::ScheduleDelayed(uint32_t delay_us) {
    hrt_call_after(&_call, delay_us, (hrt_callout)&WorkItemScheduled::schedule_trampoline, this);
}

void WorkItemScheduled::ScheduleOnInterval(uint32_t interval_us, uint32_t delay_us) {
    hrt_call_every(&_call, delay_us, interval_us, (hrt_callout)&WorkItemScheduled::schedule_trampoline, this);
}

void WorkItemScheduled::ScheduleAt(hrt_abstime time_us) {
    hrt_call_at(&_call, time_us, (hrt_callout)&WorkItemScheduled::schedule_trampoline, this);
}

void WorkItemScheduled::ScheduleClear() {
    // first clear any scheduled hrt call, then remove the item from the runnable queue
    hrt_cancel(&_call);
    WorkItem::ScheduleClear();
}

void WorkItemScheduled::print_run_status() {
    if (_call.period > 0) {
        LOG_RAW("%-29s %8.1f Hz %12.0f us (%" PRId64 " us)\n", _item_name, (double)average_rate(),
                (double)average_interval(), _call.period);

    } else {
        WorkItem::print_run_status();
    }
}

} // namespace nextpilot::workq
