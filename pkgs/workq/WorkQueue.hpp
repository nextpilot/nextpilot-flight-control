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

#include <rtthread.h>
#include <rtdbg.h>
#include "WorkQueueManager.hpp"
#include <IntrusiveSortedList.hpp>
#include <IntrusiveList.hpp>
#include <BlockingIntrusiveSortedList.hpp>
#include <IntrusiveQueue.hpp>
#include <math.h>
#include <atomic.hpp>

#define PX4_ISFINITE isfinite

namespace nextpilot {

class WorkItem;

class WorkQueue : public IntrusiveSortedListNode<WorkQueue *> {
public:
    explicit WorkQueue(const wq_config_t &wq_config);
    WorkQueue() = delete;

    ~WorkQueue();

    const wq_config_t &get_config() const {
        return _config;
    }
    const char *get_name() const {
        return _config.name;
    }

    // 将work_item添加到链表，便于遍历
    bool Attach(WorkItem *item);
    void Detach(WorkItem *item);

    // 将work_item添加到待执行列表
    void Submit(WorkItem *item);
    void Cancel(WorkItem *item);
    void Clear();

    void Run();

    void request_stop() {
        _should_exit.store(true);
    }

    void print_status(bool last = false);

    // WorkQueues sorted numerically by relative priority (-1 to -255)
    bool operator<=(const WorkQueue &rhs) const {
        return _config.relative_priority >= rhs.get_config().relative_priority;
    }

private:
    bool should_exit() const {
        return _should_exit.load();
    }

    inline void SignalWorkerThread();

#ifdef __PX4_NUTTX
    // In NuttX work can be enqueued from an ISR
    void work_lock() {
        _flags = enter_critical_section();
    }
    void work_unlock() {
        leave_critical_section(_flags);
    }
    irqstate_t _flags;
#elif __RTTHREAD__
    void work_lock() {
        rt_enter_critical();
    }
    void work_unlock() {
        rt_exit_critical();
    }
#else
    // loop as the wait may be interrupted by a signal
    void work_lock() {
        do { } while (rt_sem_take(&_qlock, RT_WAITING_FOREVER) != 0); }
    void work_unlock() {
        rt_sem_release(&_qlock);
    }
    struct rt_semaphore _qlock;
#endif

    // 等待被执行的item
    IntrusiveQueue<WorkItem *> _q;
    struct rt_semaphore        _process_lock;
    struct rt_semaphore        _exit_lock;
    const wq_config_t         &_config;
    // 该工作队列上的所有work_item
    BlockingList<WorkItem *> _work_items;
    atomic_bool   _should_exit{false};

#if defined(ENABLE_LOCKSTEP_SCHEDULER)
    int _lockstep_component{-1};
#endif // ENABLE_LOCKSTEP_SCHEDULER
};

} // namespace nextpilot
