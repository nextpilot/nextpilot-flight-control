/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "WorkItem"

#include <WorkItem.hpp>
#include <WorkQueue.hpp>
#include <WorkQueueManager.hpp>
#include <hrtimer.h>
#include <float.h>

namespace nextpilot {

WorkItem::WorkItem(const char *name, const wq_config_t &config) :
    _item_name(name) {
    if (!Init(config)) {
        LOG_E("init failed");
    }
}

WorkItem::WorkItem(const char *name, const WorkItem &work_item) :
    _item_name(name) {
    nextpilot::WorkQueue *wq = work_item._wq;

    if ((wq != nullptr) && wq->Attach(this)) {
        _wq = wq;
    }
}

WorkItem::~WorkItem() {
    Deinit();
}

bool WorkItem::Init(const wq_config_t &config) {
    // clear any existing first
    Deinit();

    nextpilot::WorkQueue *wq = WorkQueueFindOrCreate(config);

    if ((wq != nullptr) && wq->Attach(this)) {
        _wq             = wq;
        _time_first_run = 0;
        return true;
    }

    LOG_E("%s not available", config.name);
    return false;
}

void WorkItem::Deinit() {
    // remove any currently queued work
    if (_wq != nullptr) {
        // prevent additional insertions
        nextpilot::WorkQueue *wq_temp = _wq;
        _wq                           = nullptr;

        // remove any queued work
        // 从调度中移除（不再进行调度了）
        wq_temp->Cancel(this);
        // 从列表中移除（只是用来遍历）
        wq_temp->Detach(this);
    }
}

void WorkItem::ScheduleClear() {
    if (_wq != nullptr) {
        _wq->Cancel(this);
    }
}

float WorkItem::elapsed_time() const {
    return hrt_elapsed_time(&_time_first_run) / 1e6f;
}

float WorkItem::average_rate() const {
    const float rate = _run_count / elapsed_time();

    if ((_run_count > 1) && isfinite(rate)) {
        return rate;
    }

    return 0.f;
}

float WorkItem::average_interval() const {
    const float rate     = average_rate();
    const float interval = 1e6f / rate;

    if ((rate > FLT_EPSILON) && isfinite(interval)) {
        return roundf(interval);
    }

    return 0.f;
}

void WorkItem::print_run_status() {
    LOG_RAW("%-29s %8.1f Hz %12.0f us\n", _item_name, (double)average_rate(), (double)average_interval());

    // reset statistics
    _run_count = 0;
}

} // namespace nextpilot
