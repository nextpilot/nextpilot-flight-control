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

namespace nextpilot::workq {

/**
 * @class WorkItemSingleShot
 * Run a method on a specific work queue and wait for it to exit.
 * Example usage:
 *   WorkItemSingleShot initializer(device_bus_to_wq(device_id.devid), instantiate, &data);
 *   initializer.ScheduleNow();
 *   initializer.wait();
 */
class WorkItemSingleShot : public WorkItem {
public:
    using worker_method = void (*)(void *arg);

    WorkItemSingleShot(const wq_config_t &config, worker_method method, void *argument);
    WorkItemSingleShot(const WorkItem &work_item, worker_method method, void *argument);

    ~WorkItemSingleShot();

    WorkItemSingleShot() = delete;

    // no copy, assignment, move, move assignment
    WorkItemSingleShot(const WorkItemSingleShot &)            = delete;
    WorkItemSingleShot &operator=(const WorkItemSingleShot &) = delete;
    WorkItemSingleShot(WorkItemSingleShot &&)                 = delete;
    WorkItemSingleShot &operator=(WorkItemSingleShot &&)      = delete;

    void wait();

protected:
    void Run() override;

private:
    void               *_argument;
    worker_method       _method;
    struct rt_semaphore _sem;
};

} // namespace nextpilot::workq
