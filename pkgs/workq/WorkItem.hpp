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

#include "WorkQueueManager.hpp"
#include "WorkQueue.hpp"
#include <IntrusiveQueue.hpp>
#include <IntrusiveSortedList.hpp>
#include <hrtimer.h>
#include <string.h>

namespace nextpilot {

class WorkItem : public IntrusiveSortedListNode<WorkItem *>, public IntrusiveQueueNode<WorkItem *> {
public:
    WorkItem() = delete;

    // no copy, assignment, move, move assignment
    WorkItem(const WorkItem &)            = delete;
    WorkItem &operator=(const WorkItem &) = delete;
    WorkItem(WorkItem &&)                 = delete;
    WorkItem &operator=(WorkItem &&)      = delete;

    // WorkItems sorted by name
    bool operator<=(const WorkItem &rhs) const {
        return (strcmp(ItemName(), rhs.ItemName()) <= 0);
    }

    inline void ScheduleNow() {
        if (_wq != nullptr) {
            _wq->Submit(this);
        }
    }

    virtual void print_run_status();

    /**
     * Switch to a different WorkQueue.
     * NOTE: Caller is responsible for synchronization.
     *
     * @param config The WorkQueue configuration (see WorkQueueManager.hpp).
     * @return true if initialization was successful
     */
    bool ChangeWorkQueue(const wq_config_t &config) {
        return Init(config);
    }

    const char *ItemName() const {
        return _item_name;
    }

protected:
    explicit WorkItem(const char *name, const wq_config_t &config);

    explicit WorkItem(const char *name, const WorkItem &work_item);

    virtual ~WorkItem();

    /**
     * Cancel work item from the runnable queue, if it's there
     */
    void ScheduleClear();

protected:
    void RunPreamble() {
        if (_run_count == 0) {
            _time_first_run = hrt_absolute_time();
            _run_count      = 1;

        } else {
            _run_count++;
        }
    }

    friend void  WorkQueue::Run();
    virtual void Run() = 0;

    /**
     * Initialize WorkItem given a WorkQueue config. This call
     * can also be used to switch to a different WorkQueue.
     * NOTE: Caller is responsible for synchronization.
     *
     * @param config The WorkQueue configuration (see WorkQueueManager.hpp).
     * @return true if initialization was successful
     */
    bool Init(const wq_config_t &config);
    void Deinit();

    float elapsed_time() const;
    float average_rate() const;
    float average_interval() const;

    hrt_abstime _time_first_run{0};
    const char *_item_name;
    uint32_t    _run_count{0};

private:
    WorkQueue *_wq{nullptr};
};

} // namespace nextpilot
