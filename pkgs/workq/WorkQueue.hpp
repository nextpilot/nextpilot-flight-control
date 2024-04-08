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

    // 将work_item添加到链表，用于遍历
    bool Attach(WorkItem *item);
    // 将work_item从链表移除，该work_item不再属于这个工作队列了
    void Detach(WorkItem *item);

    // 将work_item添加到待执行队列
    void Submit(WorkItem *item);
    // 将work_item从执行队列中移除
    void Cancel(WorkItem *item);
    // 清空执行队列
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

#ifdef __RTTHREAD__
    void work_lock() {
        rt_enter_critical();
    }
    void work_unlock() {
        rt_exit_critical();
    }
#else
    // loop as the wait may be interrupted by a signal
    void work_lock() {
        rt_sem_take(&_qlock, RT_WAITING_FOREVER);
    }
    void work_unlock() {
        rt_sem_release(&_qlock);
    }
    struct rt_semaphore _qlock;
#endif

    // 等待被执行的item
    IntrusiveQueue<WorkItem *> _q;
    // 工作队列线程激活信号量，当有新的item加入调度，就激活该线程
    struct rt_semaphore _process_lock;
    // 工作队列线程退出信号
    struct rt_semaphore _exit_lock;
    const wq_config_t  &_config;
    // 该工作队列上的所有work_item
    BlockingList<WorkItem *> _work_item_list;
    atomic_bool              _should_exit{false};

#if defined(ENABLE_LOCKSTEP_SCHEDULER)
    int _lockstep_component{-1};
#endif // ENABLE_LOCKSTEP_SCHEDULER
};

} // namespace nextpilot
